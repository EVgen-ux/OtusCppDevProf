#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <boost/filesystem.hpp>

namespace bayan {

namespace bfs = boost::filesystem;

enum class HashAlgorithm {
    CRC32,
    MD5
};

struct ScanSettings {
    std::vector<bfs::path> target_directories;
    std::vector<bfs::path> excluded_directories;
    std::vector<std::string> filename_patterns;
    size_t scan_depth = 0;
    int64_t min_file_size = 0;
    size_t hash_block_size = 4096;
    HashAlgorithm hash_algorithm = HashAlgorithm::MD5;
};

class FileHasher {
public:
    virtual ~FileHasher() = default;
    virtual void update(const char* data, size_t size) = 0;
    virtual bool compare(const FileHasher& other) const = 0;
    virtual std::string get_hash() const = 0;
    virtual void reset() = 0;
};

class DuplicateFinder {
public:
    explicit DuplicateFinder(const ScanSettings& settings);
    void execute_scan();
    
private:
    struct FileMetadata {
        bfs::path path;
        uint64_t size;
        std::unique_ptr<FileHasher> hasher;
        
        FileMetadata(bfs::path p, uint64_t s) : path(std::move(p)), size(s) {}
    };
    
    using FileGroup = std::vector<FileMetadata>;
    using DuplicateGroups = std::vector<std::vector<bfs::path>>;
    
    void collect_files();
    bool should_include_file(const bfs::path& file_path, uint64_t file_size) const;
    bool matches_patterns(const std::string& filename) const;
    bool is_excluded(const bfs::path& path) const;
    
    DuplicateGroups find_duplicates();
    DuplicateGroups group_by_size();
    DuplicateGroups group_by_content(DuplicateGroups size_groups);
    
    void output_results(const DuplicateGroups& duplicates) const;
    
    std::unique_ptr<FileHasher> create_hasher() const;
    
    ScanSettings settings_;
    std::vector<FileMetadata> files_;
};

std::unique_ptr<FileHasher> create_hasher(HashAlgorithm algorithm);

} // namespace bayan