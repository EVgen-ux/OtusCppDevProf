#include <iostream>
#include <boost/asio.hpp>
#include "server.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    try {
        boost::asio::io_context io_context;
        Server server(io_context, std::atoi(argv[1]));
        
        std::cout << "Server started on port " << argv[1] << std::endl;
        std::cout << "Waiting for connections..." << std::endl;
        
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
