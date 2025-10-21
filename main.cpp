#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <iomanip>

#include "helpers.h"
#include "tf_classifier.h"

const size_t width = 28;
const size_t height = 28;
const size_t output_dim = 10;

using namespace mnist;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./fasion_mnist <test.csv_path> <model_path> \n";
        return 1;
    }

    std::string test_data_path{argv[1]};
    std::string model_path{argv[2]};
    
    auto clf = TfClassifier{model_path, width, height};
    auto features = TfClassifier::features_t{};

    std::ifstream test_data{test_data_path};
    if (!test_data.is_open()) {
        std::cerr << "Unable to open file " << test_data_path << std::endl;
        return 1;
    }
    

    std::string header;
    std::getline(test_data, header);
    
    std::vector<char> matches;
    size_t line_count = 0;
    size_t correct_predictions = 0;
    
    for (;;) {
        size_t y_true;
        test_data >> y_true;
        
        if (!read_features(test_data, features)) {
            break;
        }
        
        if (features.size() != width * height) {
            std::cerr << "Error: Expected " << (width * height) << " features, but got " << features.size() << " at line " << line_count << std::endl;
            continue;
        }
        
        try {
            auto y_pred = clf.predict(features);           
            bool match = (y_true == y_pred);
            matches.push_back(static_cast<char>(match));
            if (match) correct_predictions++;
            
        } catch (const std::exception& e) {
            std::cerr << "Error predicting line " << line_count << ": " << e.what() << std::endl;
            matches.push_back(0);
        }
        
        line_count++;       
    }

    if (matches.empty()) {
        std::cerr << "No samples were processed!" << std::endl;
        return 1;
    }

    double accuracy = static_cast<double>(correct_predictions) / matches.size();
    std::cout << "Accuracy = " << std::fixed << std::setprecision(4) << accuracy << std::endl;
    
    return 0;
}