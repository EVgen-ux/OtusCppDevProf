#include "../include/async_bulk/bulk.h"
#include <iostream>
#include <fstream>

Bulk::Bulk(size_t blockSize) : blockSize_(blockSize) {
    baseTimestamp_ = std::chrono::system_clock::now();
}

void Bulk::processCommand(const std::string& command) {
    if (command == "{") {
        if (dynamicNestingLevel_ == 0) {
            processRegularCommands();
        }
        dynamicNestingLevel_++;
        return;
    }

    if (command == "}") {
        if (dynamicNestingLevel_ > 0) {
            dynamicNestingLevel_--;
            if (dynamicNestingLevel_ == 0) {
                processDynamicCommands();
            }
        }
        return;
    }

    if (dynamicNestingLevel_ > 0) {
        dynamicCommands_.push_back(command);
    } else {
        regularCommands_.push_back(command);
        if (regularCommands_.size() >= blockSize_) {
            processRegularCommands();
        }
    }
}

void Bulk::finalize() {
    if (dynamicNestingLevel_ == 0 && !regularCommands_.empty()) {
        processRegularCommands();
    }
}

std::vector<std::string> Bulk::getOutput() {
    std::vector<std::string> output;
    std::swap(output, readyOutput_);
    return output;
}

void Bulk::processRegularCommands() {
    if (regularCommands_.empty()) return;
    
    readyOutput_ = regularCommands_;
    regularCommands_.clear();
}

void Bulk::processDynamicCommands() {
    if (dynamicCommands_.empty()) return;
    
    // Формируем вывод из динамических команд
    readyOutput_ = dynamicCommands_;
    dynamicCommands_.clear();
}