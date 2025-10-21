#include "helpers.h"
#include <fstream>
#include <sstream>
#include <iterator>

namespace mnist {

Eigen::MatrixXf read_mat_from_stream(size_t rows, size_t cols, std::istream& stream) {
    Eigen::MatrixXf res(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            float val;
            stream >> val;
            res(i, j) = val;
        }
    }
    return res;
}

Eigen::MatrixXf read_mat_from_file(size_t rows, size_t cols, const std::string& filepath) {
    std::ifstream stream{filepath};
    return read_mat_from_stream(rows, cols, stream);
}

std::vector<std::string> split(const std::string& s, const char delimiter) {
    std::stringstream sstream{s};
    std::string segment;
    std::vector<std::string> seglist;

    while(std::getline(sstream, segment, delimiter))
    {
        seglist.push_back(segment);
    }

    return seglist;
}

bool read_features(std::istream& stream, Classifier::features_t& features) {
    std::string line;
    std::getline(stream, line);

    features.clear();

    auto features_s = split(line, ',');
    for (size_t i = 1; i < features_s.size(); ++i) {
        features.push_back(std::stof(features_s[i]));
    }

    return stream.good();
}

std::vector<float> read_vector(std::istream& stream) {
    std::vector<float> result;

    std::copy(std::istream_iterator<float>(stream),
              std::istream_iterator<float>(),
              std::back_inserter(result));
    return result;
}

}
