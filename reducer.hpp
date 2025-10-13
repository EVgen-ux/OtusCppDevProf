#pragma once

#include <forward_list>
#include <functional>
#include <future>
#include <string>
#include <unordered_map>
#include <vector>

class Reducer {
public:
    using reduce_input_t = std::forward_list<std::string>;
    using reduce_result_t = std::unordered_map<std::string, double>;
    using reduce_fn_t = std::function<reduce_result_t(reduce_input_t::iterator,
                                                      reduce_input_t::iterator)>;

    Reducer(size_t num_threads, std::vector<reduce_input_t>& data, reduce_fn_t reduce_function);
    reduce_result_t run();

private:
    size_t num_threads_;
    std::vector<reduce_input_t>& data_;
    reduce_fn_t reduce_function_;
};