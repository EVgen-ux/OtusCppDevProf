#include <iostream>
#include <boost/asio.hpp>
#include "client.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        return 1;
    }

    try {
        boost::asio::io_context io_context;
        Client client(io_context, argv[1], argv[2]);
        
        std::cout << "Available commands:" << std::endl;
        std::cout << "  INSERT <A|B> <id> <name>    - Add row to table" << std::endl;
        std::cout << "  TRUNCATE <A|B>              - Clear table" << std::endl;
        std::cout << "  INTERSECTION                - Get common rows" << std::endl;
        std::cout << "  SYMMETRIC_DIFFERENCE        - Get different rows" << std::endl;
        std::cout << "Type 'quit' to exit" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        std::string input;
        while (std::getline(std::cin, input)) {
            if (input == "quit" || input == "exit") break;
            if (!input.empty()) {
                client.send_command(input);
            }
        }
        
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}