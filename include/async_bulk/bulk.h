#pragma once

#include <vector>
#include <string>
#include <chrono>

class Bulk {
public:
    Bulk(size_t blockSize);
    void processCommand(const std::string& command);
    void finalize();
    std::vector<std::string> getOutput();

private:
    void processRegularCommands();
    void processDynamicCommands();
    void outputCurrentBulk();

    size_t blockSize_;
    int dynamicNestingLevel_ = 0;
    std::vector<std::string> regularCommands_;
    std::vector<std::string> dynamicCommands_;
    std::vector<std::string> readyOutput_;
    std::chrono::system_clock::time_point baseTimestamp_;
};