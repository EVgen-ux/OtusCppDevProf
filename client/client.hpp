#pragma once

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Client {
public:
    Client(boost::asio::io_context& io_context, const std::string& host, const std::string& port);
    void send_command(const std::string& command);

private:
    tcp::socket socket_;
};