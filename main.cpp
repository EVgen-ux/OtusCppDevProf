#include "bulk.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "The program accepts exaxtly one integral argument" << std::endl;
        std::cout << "Example: ./bulk 3" << std::endl;
        return 1;
    }

    size_t blockSize;
    try {
        blockSize = std::stoul(argv[1]);
    } catch (...) {
        std::cerr << "Error: Invalid block size\n";
        return 1;
    }

    Bulk bulk(blockSize);

    std::ifstream commandsFile("commands.txt");
    
    if (!commandsFile.is_open()) {
        std::cerr << "Error: Could not open commands.txt\n";
        return 1;
    }

    std::string command;

    while (std::getline(commandsFile, command)) {
        bulk.processCommand(command);
    }

    return 0;
}