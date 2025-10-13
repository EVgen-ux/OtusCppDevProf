#pragma once

#include <forward_list>
#include <functional>
#include <future>
#include <string>
#include <vector>

class Mapper {
public:
    using map_fn_t = std::function<std::string(std::string)>;
    
    Mapper(size_t num_threads, const std::vector<std::string>& data, map_fn_t map_function);
    std::vector<std::forward_list<std::string>> run();

private:
    size_t num_threads_;
    std::vector<std::string> data_;
    map_fn_t map_function_;
};