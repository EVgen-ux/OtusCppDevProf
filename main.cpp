#include <iostream>
#include <string>
#include <cmath>

#include "mapper.hpp"
#include "reducer.hpp"
#include "mapreduce_operations.hpp"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: mapreduce <filename> <n_mappers> <n_reducers>\n";
        return 1;
    }

    std::string file_name{argv[1]};
    size_t n_mappers = std::stoi(argv[2]);
    size_t n_reducers = std::stoi(argv[3]);

    auto file_chunks = read_file_by_chunks(file_name, n_mappers);

    // === РАСЧЕТ СРЕДНЕГО ЗНАЧЕНИЯ ===
    {
        Mapper mapper{n_mappers, file_chunks, extract_price};
        auto map_result = mapper.run();
        auto merged_data = merge(map_result);
        auto reduce_input = prepare_reduce_data(merged_data, n_reducers);

        Reducer reducer{n_reducers, reduce_input, calculate_mean};
        auto reduce_result = reducer.run();

        if (reduce_result.count("sum") && reduce_result.count("count")) {
            double mean = reduce_result["sum"] / reduce_result["count"];
            std::cout << "Average price: " << mean << std::endl;
        }
    }

    // === РАСЧЕТ ДИСПЕРСИИ ===
    {
        Mapper mapper{n_mappers, file_chunks, extract_price_squared};
        auto map_result = mapper.run();
        auto merged_data = merge(map_result);
        auto reduce_input = prepare_reduce_data(merged_data, n_reducers);

        Reducer reducer{n_reducers, reduce_input, calculate_variance};
        auto reduce_result = reducer.run();

        if (reduce_result.count("sum") && reduce_result.count("sum_squares") && reduce_result.count("count")) {
            double sum = reduce_result["sum"];
            double sum_squares = reduce_result["sum_squares"];
            int count = reduce_result["count"];
            
            double mean = sum / count;
            double variance = (sum_squares / count) - (mean * mean);
            
            std::cout << "Variance: " << variance << std::endl;
            std::cout << "Standard deviation: " << std::sqrt(variance) << std::endl;
        }
    }

    return 0;
}