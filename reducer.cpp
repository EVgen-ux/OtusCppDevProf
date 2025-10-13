#include "reducer.hpp"
#include <stdexcept>

Reducer::Reducer(size_t num_threads, std::vector<reduce_input_t>& data,
                 reduce_fn_t reduce_function)
    : num_threads_{num_threads}, data_{data}, reduce_function_{reduce_function} {
    if (num_threads != data.size()) {
        throw std::runtime_error("data.size() != num_threads");
    }
}

Reducer::reduce_result_t Reducer::run() {

    std::vector<std::future<reduce_result_t>> jobs;
    for (size_t i = 0; i < num_threads_; ++i) {
        jobs.push_back(std::async(std::launch::async,
            [this, i]() {
                return reduce_function_(data_[i].begin(), data_[i].end());
            }));
    }

    std::vector<reduce_result_t> job_results;
    for (auto& job : jobs) {
        job_results.push_back(job.get());
    }

    reduce_result_t final_result;
    for (auto& job_result : job_results) {
        for (const auto& [key, value] : job_result) {
            final_result[key] += value;
        }
    }

    return final_result;
}