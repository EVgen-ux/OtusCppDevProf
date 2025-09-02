#include <iostream>
#include <fstream>
#include <string>
#include <thread>       
#include <chrono>       


#include <async_bulk/async.h>

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

    auto ctx = AsyncBulk::connect(blockSize);

    std::ifstream commandsFile("commands.txt");
    
    if (!commandsFile.is_open()) {
        std::cerr << "Error: Could not open commands.txt\n";
        AsyncBulk::disconnect(ctx);
        return 1;
    }

    std::string command;

    while (std::getline(commandsFile, command)) {
        AsyncBulk::receive(ctx, command);
    }

    AsyncBulk::disconnect(ctx);
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    AsyncBulk::shutdown();

    return 0;
}