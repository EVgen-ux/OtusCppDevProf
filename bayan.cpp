#include "bayan.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <fnmatch.h> 
#include <cctype> 


namespace bayan {

std::unique_ptr<FileHasher> create_hasher(HashAlgorithm algorithm);

DuplicateFinder::DuplicateFinder(const ScanSettings& settings) 
    : settings_(settings) {}

void DuplicateFinder::execute_scan() {
    collect_files();
    auto duplicates = find_duplicates();
    output_results(duplicates);
}

void DuplicateFinder::collect_files() {
    for (const auto& dir : settings_.target_directories) {
        if (!bfs::exists(dir)) continue;
        
        auto options = bfs::directory_options::skip_permission_denied;
        auto iterator = bfs::recursive_directory_iterator(dir, options);
        
        for (const auto& entry : iterator) {
            if (bfs::is_regular_file(entry.status()) && !bfs::is_symlink(entry.symlink_status())) {
                const auto file_size = bfs::file_size(entry.path());
                if (should_include_file(entry.path(), file_size)) {
                    files_.emplace_back(entry.path(), file_size);
                }
            }
            
            if (bfs::is_directory(entry.status())) {
                if (iterator.depth() >= settings_.scan_depth) {
                    iterator.disable_recursion_pending();
                }
                if (is_excluded(entry.path())) {
                    iterator.disable_recursion_pending();
                }
            }
        }
    }
}

bool DuplicateFinder::should_include_file(const bfs::path& file_path, uint64_t file_size) const {
    if (file_size < settings_.min_file_size) return false;
    if (!matches_patterns(file_path.filename().string())) return false;
    return !is_excluded(file_path);
}

bool DuplicateFinder::matches_patterns(const std::string& filename) const {
    if (settings_.filename_patterns.empty()) return true;
    

    std::string filename_lower = filename;
    std::transform(filename_lower.begin(), filename_lower.end(), filename_lower.begin(),
                  [](unsigned char c) { return std::tolower(c); });
    
    for (const auto& pattern : settings_.filename_patterns) {

        std::string pattern_lower = pattern;
        std::transform(pattern_lower.begin(), pattern_lower.end(), pattern_lower.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        
        if (fnmatch(pattern_lower.c_str(), filename_lower.c_str(), FNM_CASEFOLD) == 0) {
            return true;
        }
    }
    return false;
}


bool DuplicateFinder::is_excluded(const bfs::path& path) const {
    auto canonical_path = bfs::canonical(path);
    for (const auto& excluded : settings_.excluded_directories) {
        if (bfs::equivalent(canonical_path, bfs::canonical(excluded))) {
            return true;
        }
    }
    return false;
}

DuplicateFinder::DuplicateGroups DuplicateFinder::find_duplicates() {
    auto size_groups = group_by_size();
    return group_by_content(std::move(size_groups));
}

DuplicateFinder::DuplicateGroups DuplicateFinder::group_by_size() {
    std::sort(files_.begin(), files_.end(), 
        [](const auto& a, const auto& b) { return a.size < b.size; });
    
    DuplicateGroups groups;
    auto it = files_.begin();
    
    while (it != files_.end()) {
        auto group_end = std::find_if_not(it, files_.end(), 
            [size = it->size](const auto& file) { return file.size == size; });
        
        if (std::distance(it, group_end) > 1) {
            std::vector<bfs::path> group_paths;
            std::transform(it, group_end, std::back_inserter(group_paths),
                [](const auto& file) { return file.path; });
            groups.push_back(std::move(group_paths));
        }
        
        it = group_end;
    }
    
    return groups;
}

DuplicateFinder::DuplicateGroups DuplicateFinder::group_by_content(DuplicateGroups size_groups) {
    DuplicateGroups content_groups;
    
    for (auto& group : size_groups) {
        std::unordered_map<std::string, std::vector<bfs::path>> hash_groups;
        
        for (const auto& path : group) {
            auto hasher = create_hasher();
            std::ifstream file(path.native(), std::ios::binary);
            
            if (!file) continue;
            
            std::vector<char> buffer(settings_.hash_block_size);
            while (file.read(buffer.data(), buffer.size())) {
                hasher->update(buffer.data(), file.gcount());
            }
            hasher->update(buffer.data(), file.gcount());
            
            hash_groups[hasher->get_hash()].push_back(path);
        }
        
        for (auto& hash_group : hash_groups) {
            if (hash_group.second.size() > 1) {
                content_groups.push_back(std::move(hash_group.second));
            }
        }
    }
    
    return content_groups;
}

void DuplicateFinder::output_results(const DuplicateGroups& duplicates) const {
    for (const auto& group : duplicates) {
        std::cout << "Duplicate group found (" << group.size() << " files):\n";
        for (const auto& path : group) {
            std::cout << "  " << path.string() << "\n";
        }
        std::cout << std::endl;
    }
    
    if (duplicates.empty()) {
        std::cout << "No duplicate files found." << std::endl;
    }
}

std::unique_ptr<FileHasher> DuplicateFinder::create_hasher() const {
    return ::bayan::create_hasher(settings_.hash_algorithm);
}


class CRC32Hasher : public FileHasher {
public:
    void update(const char* data, size_t size) override {
        crc_.process_bytes(data, size);
    }
    
    bool compare(const FileHasher& other) const override {
        try {
            const auto& other_crc = dynamic_cast<const CRC32Hasher&>(other);
            return crc_.checksum() == other_crc.crc_.checksum();
        } catch (...) {
            return false;
        }
    }
    
    std::string get_hash() const override {
        return std::to_string(crc_.checksum());
    }
    
    void reset() override {
        crc_.reset();
    }
    
private:
    boost::crc_32_type crc_;
};

class MD5Hasher : public FileHasher {
public:
    void update(const char* data, size_t size) override {
        hash_.process_bytes(data, size);
    }
    
    bool compare(const FileHasher& other) const override {
        try {
            const auto& other_md5 = dynamic_cast<const MD5Hasher&>(other);
            
            MD5Hasher this_copy = *this;
            MD5Hasher other_copy = other_md5;
            
            boost::uuids::detail::md5::digest_type this_digest, other_digest;
            this_copy.hash_.get_digest(this_digest);
            other_copy.hash_.get_digest(other_digest);
            
            return std::memcmp(this_digest, other_digest, sizeof(this_digest)) == 0;
        } catch (...) {
            return false;
        }
    }
    
    std::string get_hash() const override {

        MD5Hasher copy = *this;
        boost::uuids::detail::md5::digest_type digest;
        copy.hash_.get_digest(digest);
        

        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (int i = 0; i < sizeof(digest); ++i) {
            ss << std::setw(2) << static_cast<unsigned int>(
                reinterpret_cast<const unsigned char*>(digest)[i]);
        }
        
        return ss.str();
    }
    
    void reset() override {

        hash_ = boost::uuids::detail::md5();
    }
    
private:
    boost::uuids::detail::md5 hash_;
};

std::unique_ptr<FileHasher> create_hasher(HashAlgorithm algorithm) {
    switch (algorithm) {
        case HashAlgorithm::CRC32:
            return std::make_unique<CRC32Hasher>();
        case HashAlgorithm::MD5:
            return std::make_unique<MD5Hasher>();
        default:
            return std::make_unique<MD5Hasher>();
    }
}

} // namespace bayan