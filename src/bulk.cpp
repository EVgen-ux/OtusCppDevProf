#include "../include/async_bulk/bulk.h"
#include <iostream>
#include <fstream>

Bulk::Bulk(size_t blockSize) : blockSize_(blockSize) {
    baseTimestamp_ = std::chrono::system_clock::now();
}

void Bulk::processCommand(const std::string& command) {
    if (command == "{") {
        if (dynamicNestingLevel_ == 0) {
            // Если входим в первый динамический блок, обрабатываем накопленные регулярные команды
            processRegularCommands();
        }
        dynamicNestingLevel_++;
        return;
    }

    if (command == "}") {
        if (dynamicNestingLevel_ > 0) {
            dynamicNestingLevel_--;
            if (dynamicNestingLevel_ == 0) {
                // Если вышли из всех динамических блоков, обрабатываем накопленные команды
                processDynamicCommands();
            }
        }
        return;
    }

    if (dynamicNestingLevel_ > 0) {
        // Команды внутри динамических блоков
        dynamicCommands_.push_back(command);
    } else {
        // Регулярные команды
        regularCommands_.push_back(command);
        if (regularCommands_.size() >= blockSize_) {
            processRegularCommands();
        }
    }
}

void Bulk::finalize() {
    if (dynamicNestingLevel_ == 0) {
        // Если не внутри динамического блока, обрабатываем оставшиеся регулярные команды
        processRegularCommands();
    }
    // Если внутри динамического блока, команды не обрабатываются (по спецификации)
}

std::vector<std::string> Bulk::getOutput() {
    std::vector<std::string> output;
    std::swap(output, readyOutput_);
    return output;
}

void Bulk::processRegularCommands() {
    if (regularCommands_.empty()) return;
    
    // Формируем вывод из регулярных команд
    readyOutput_ = regularCommands_;
    regularCommands_.clear();
}

void Bulk::processDynamicCommands() {
    if (dynamicCommands_.empty()) return;
    
    // Формируем вывод из динамических команд
    readyOutput_ = dynamicCommands_;
    dynamicCommands_.clear();
}