#include "client.hpp"
#include <iostream>

Client::Client(boost::asio::io_context& io_context, const std::string& host, const std::string& port)
    : socket_(io_context) {
    tcp::resolver resolver(io_context);
    boost::asio::connect(socket_, resolver.resolve(host, port));
    std::cout << "Connected to server!" << std::endl;
}

void Client::send_command(const std::string& command) {
    boost::asio::write(socket_, boost::asio::buffer(command + "\n"));
    
    char response[1024];
    size_t length = socket_.read_some(boost::asio::buffer(response));
    std::cout << std::string(response, length) << std::endl; 
}