#include "mapper.hpp"
#include <stdexcept>
#include <algorithm>

Mapper::Mapper(size_t num_threads, const std::vector<std::string>& data, 
               map_fn_t map_function)
    : num_threads_{num_threads}, data_{data}, map_function_{map_function} {
    if (num_threads == 0) {
        throw std::runtime_error("Number of threads must be positive");
    }
}

std::vector<std::forward_list<std::string>> Mapper::run() {

    std::vector<std::vector<std::string>> thread_data(num_threads_);
    for (size_t i = 0; i < data_.size(); ++i) {
        thread_data[i % num_threads_].push_back(data_[i]);
    }


    std::vector<std::future<std::forward_list<std::string>>> jobs;
    for (size_t i = 0; i < num_threads_; ++i) {
        jobs.push_back(std::async(std::launch::async,
            [this, &thread_data, i]() {
                std::forward_list<std::string> result;
                for (const auto& line : thread_data[i]) {
                    auto mapped = map_function_(line);
                    if (!mapped.empty()) {
                        result.push_front(std::move(mapped));
                    }
                }
                return result;
            }));
    }

    std::vector<std::forward_list<std::string>> result;
    for (auto& job : jobs) {
        result.push_back(job.get());
    }

    return result;
}