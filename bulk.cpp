#include "bulk.h"
#include <iostream>
#include <fstream>

Bulk::Bulk(size_t blockSize) : blockSize_(blockSize) {
    baseTimestamp_ = std::chrono::system_clock::now();
}

void Bulk::processCommand(const std::string& command) {
    std::cout << command << std::endl;
    
    if (command == "{") {
        if (dynamicNestingLevel_ == 0) {
            RegularCommands();
        }
        dynamicNestingLevel_++;
        return;
    }

    if (command == "}") {
        if (dynamicNestingLevel_ > 0) {
            dynamicNestingLevel_--;
            if (dynamicNestingLevel_ == 0) {
                DynamicCommands();
            }
        }
        return;
    }

    if (dynamicNestingLevel_ > 0) {
        dynamicCommands_.push_back(command);
    } else {
        regularCommands_.push_back(command);
        if (regularCommands_.size() >= blockSize_) {
            RegularCommands();
        }
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void Bulk::RegularCommands() {
    if (regularCommands_.empty()) return;
    outputBulk(regularCommands_);
    regularCommands_.clear();
}

void Bulk::DynamicCommands() {
    if (dynamicCommands_.empty()) return;
    outputBulk(dynamicCommands_);
    dynamicCommands_.clear();
}

void Bulk::outputBulk(const std::vector<std::string>& commands) {
    std::cout << "bulk: ";
    printCommands(std::cout, commands);

    auto timestamp = std::chrono::system_clock::to_time_t(baseTimestamp_);
    std::string filename = "bulk" + std::to_string(timestamp + fileCounter_++) + ".log";
    std::ofstream logFile(filename);
    if (logFile) {
        logFile << "bulk: ";
        printCommands(logFile, commands);
    }
}

void Bulk::printCommands(std::ostream& os, const std::vector<std::string>& commands) {
    for (size_t i = 0; i < commands.size(); ++i) {
        if (i != 0) os << ", ";
        os << commands[i];
    }
    os << std::endl;
}