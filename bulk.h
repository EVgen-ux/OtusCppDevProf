#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <thread>

class Bulk {
public:
    Bulk(size_t blockSize);
    void processCommand(const std::string& command);

private:
    void RegularCommands();
    void DynamicCommands();
    void outputBulk(const std::vector<std::string>& commands);
    void printCommands(std::ostream& os, const std::vector<std::string>& commands);

    const size_t blockSize_;
    std::vector<std::string> regularCommands_;
    std::vector<std::string> dynamicCommands_;
    size_t dynamicNestingLevel_ = 0;
    std::chrono::system_clock::time_point baseTimestamp_;
    size_t fileCounter_ = 0;
};
