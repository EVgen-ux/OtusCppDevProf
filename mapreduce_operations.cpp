#include "mapreduce_operations.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// File operations
std::vector<std::string> read_file_by_chunks(const std::string& file_name, 
                                           size_t num_chunks) {
    std::ifstream file{file_name};
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + file_name);
    }

    file.seekg(0, std::ios::end);
    auto file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (num_chunks == 0) {
        throw std::invalid_argument("Number of chunks must be positive");
    }

    std::vector<std::string> chunks;
    std::vector<std::streampos> boundaries;


    auto chunk_size = file_size / num_chunks;
    for (size_t i = 0; i < num_chunks; ++i) {
        auto pos = std::min(static_cast<std::streampos>(i * chunk_size), file_size);
        if (i > 0) {
            file.seekg(pos);
            std::string line;
            std::getline(file, line);
            pos = file.tellg();
        }
        boundaries.push_back(pos);
    }
    boundaries.push_back(file_size);

    // Читаем чанки
    for (size_t i = 0; i < num_chunks; ++i) {
        auto chunk_start = boundaries[i];
        auto chunk_end = boundaries[i + 1];
        auto chunk_length = chunk_end - chunk_start;

        if (chunk_length <= 0) continue;

        file.seekg(chunk_start);
        std::string chunk(chunk_length, '\0');
        file.read(&chunk[0], chunk_length);
        chunks.push_back(std::move(chunk));
    }

    file.close();
    return chunks;
}

// Map functions
std::string extract_price(std::string source) {
    std::stringstream ss(source);
    std::string token;
    int column = 0;
    
    while (std::getline(ss, token, ',')) {
        if (column == 9) {
            return token;
        }
        column++;
    }
    return "";
}

std::string extract_price_squared(std::string source) {
    std::string price_str = extract_price(source);
    if (price_str.empty()) return "";
    
    try {
        double price = std::stod(price_str);
        double squared = price * price;
        return price_str + "," + std::to_string(squared);
    } catch (...) {
        return "";
    }
}

// Reduce functions
std::unordered_map<std::string, double> calculate_mean(
    std::forward_list<std::string>::iterator begin,
    std::forward_list<std::string>::iterator end) {
    
    std::unordered_map<std::string, double> result;
    double sum = 0.0;
    int count = 0;

    for (; begin != end; ++begin) {
        if (begin->empty()) continue;
        
        try {
            double price = std::stod(*begin);
            sum += price;
            count++;
        } catch (...) {
            // Игнорируем некорректные значения
        }
    }

    if (count > 0) {
        result["sum"] = sum;
        result["count"] = count;
    }

    return result;
}

std::unordered_map<std::string, double> calculate_variance(
    std::forward_list<std::string>::iterator begin,
    std::forward_list<std::string>::iterator end) {
    
    std::unordered_map<std::string, double> result;
    double sum = 0.0;
    double sum_squares = 0.0;
    int count = 0;

    for (; begin != end; ++begin) {
        if (begin->empty()) continue;
        
        std::stringstream ss(*begin);
        std::string price_str, squared_str;
        
        if (std::getline(ss, price_str, ',') && std::getline(ss, squared_str)) {
            try {
                double price = std::stod(price_str);
                double squared = std::stod(squared_str);
                sum += price;
                sum_squares += squared;
                count++;
            } catch (...) {
                // Игнорируем некорректные значения
            }
        }
    }

    if (count > 0) {
        result["sum"] = sum;
        result["sum_squares"] = sum_squares;
        result["count"] = count;
    }

    return result;
}

// Utility functions
std::forward_list<std::string> merge(
    std::vector<std::forward_list<std::string>> lists) {
    
    std::forward_list<std::string> result;
    
    for (auto& list : lists) {
        for (auto& element : list) {
            if (!element.empty()) {
                result.push_front(std::move(element));
            }
        }
    }

    return result;
}

std::vector<std::forward_list<std::string>> prepare_reduce_data(
    std::forward_list<std::string>& data, size_t num_blocks) {
    
    if (num_blocks == 0) {
        throw std::invalid_argument("Number of blocks must be positive");
    }

    std::vector<std::forward_list<std::string>> result(num_blocks);
    size_t idx = 0;

    for (auto& element : data) {
        if (!element.empty()) {
            result[idx % num_blocks].push_front(std::move(element));
            ++idx;
        }
    }

    return result;
}