#include <iostream>
#include <string>
#include <thread>       
#include <chrono>       

#include <async_bulk/async.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "The program accepts exactly one integral argument" << std::endl;
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

    std::cout << "Enter commands (type 'EOF' on new line to finish):" << std::endl;
    std::string command;

    while (std::getline(std::cin, command)) {
        if (command == "EOF") {
            break;
        }
        AsyncBulk::receive(ctx, command);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    AsyncBulk::disconnect(ctx);
    
    std::this_thread::sleep_for(std::chrono::seconds(100));
    AsyncBulk::shutdown();

    return 0;
}