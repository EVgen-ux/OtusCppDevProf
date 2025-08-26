#include <iostream>
#include <boost/program_options.hpp>
#include "bayan.hpp"

using namespace std;

namespace po = boost::program_options;

bayan::HashAlgorithm parse_hash_algorithm(const std::string& str) {
    if (str == "crc32") return bayan::HashAlgorithm::CRC32;
    if (str == "md5") return bayan::HashAlgorithm::MD5;
    return bayan::HashAlgorithm::MD5;
}

int main(int argc, char* argv[]) {
    bayan::ScanSettings settings;
    
    po::options_description desc("Bayan - Duplicate File Finder");
    desc.add_options()
        ("help,h", "Show help message")
        ("include,i", po::value<std::vector<boost::filesystem::path>>()->required(), 
         "Directories to scan (required)")
        ("exclude,e", po::value<std::vector<boost::filesystem::path>>(), 
         "Directories to exclude")
        ("depth,d", po::value<size_t>()->default_value(0), 
         "Scan depth (0 = current directory only)")
        ("pattern,p", po::value<std::vector<std::string>>(), 
         "Filename patterns to include")
        ("min-size,s", po::value<int64_t>()->default_value(0), 
         "Minimum file size in bytes")
        ("block-size,b", po::value<size_t>()->default_value(4096), 
         "Hash block size in bytes")
        ("algorithm,a", po::value<std::string>()->default_value("md5"), 
         "Hash algorithm: crc32, md5");
    
    po::positional_options_description pos;
    pos.add("include", -1);
    
    try {
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv)
            .options(desc).positional(pos).run(), vm);
        
        if (vm.count("help")) {
            std::cout << "Bayan - Duplicate File Finder:\n";
            std::cout << "  -h [ --help ]                   Show help message\n";
            std::cout << "  -i [ --include ] arg            Directories to scan (required)\n";
            std::cout << "  -e [ --exclude ] arg            Directories to exclude\n";
            std::cout << "  -d [ --depth ] arg (=0)         Scan depth (0 = current directory only)\n";
            std::cout << "  -p [ --pattern ] arg            Filename patterns to include\n";
            std::cout << "  -s [ --min-size ] arg (=0)      Minimum file size in bytes\n";
            std::cout << "  -b [ --block-size ] arg (=4096) Hash block size in bytes\n";
            std::cout << "  -a [ --algorithm ] arg (=md5)   Hash algorithm: crc32, md5\n";
            return 0;
        }
        
        po::notify(vm);
        
        settings.target_directories = vm["include"].as<std::vector<boost::filesystem::path>>();
        if (vm.count("exclude")) {
            settings.excluded_directories = vm["exclude"].as<std::vector<boost::filesystem::path>>();
        }
        if (vm.count("pattern")) {
            settings.filename_patterns = vm["pattern"].as<std::vector<std::string>>();
        }
        
        settings.scan_depth = vm["depth"].as<size_t>();
        settings.min_file_size = vm["min-size"].as<int64_t>();
        settings.hash_block_size = vm["block-size"].as<size_t>();
        settings.hash_algorithm = parse_hash_algorithm(vm["algorithm"].as<std::string>());
        
        bayan::DuplicateFinder finder(settings);
        finder.execute_scan();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Bayan - Duplicate File Finder:\n";
        std::cerr << "  -h [ --help ]                   Show help message\n";
        std::cerr << "  -i [ --include ] arg            Directories to scan (required)\n";
        std::cerr << "  -e [ --exclude ] arg            Directories to exclude\n";
        std::cerr << "  -d [ --depth ] arg (=0)         Scan depth (0 = current directory only)\n";
        std::cerr << "  -p [ --pattern ] arg            Filename patterns to include\n";
        std::cerr << "  -s [ --min-size ] arg (=0)      Minimum file size in bytes\n";
        std::cerr << "  -b [ --block-size ] arg (=4096) Hash block size in bytes\n";
        std::cerr << "  -a [ --algorithm ] arg (=md5)   Hash algorithm: crc32, md5\n";
        return 1;
    }
    
    return 0;
}