#pragma once

#include <forward_list>
#include <string>
#include <vector>
#include <unordered_map>

// File operations
std::vector<std::string> read_file_by_chunks(const std::string& file_name, 
                                           size_t num_chunks);

// Map operations
std::string extract_price(std::string source);
std::string extract_price_squared(std::string source);

// Reduce operations  
std::unordered_map<std::string, double> calculate_mean(
    std::forward_list<std::string>::iterator begin,
    std::forward_list<std::string>::iterator end);

std::unordered_map<std::string, double> calculate_variance(
    std::forward_list<std::string>::iterator begin,
    std::forward_list<std::string>::iterator end);

// Data preparation utilities
std::forward_list<std::string> merge(
    std::vector<std::forward_list<std::string>> lists);

std::vector<std::forward_list<std::string>> prepare_reduce_data(
    std::forward_list<std::string>& data, size_t num_blocks);