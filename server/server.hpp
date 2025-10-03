#pragma once

#include <boost/asio.hpp>
#include "database/database.hpp"
#include <memory>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, Database& db);
    void start();

private:
    void do_read();
    void do_write(const std::string& response);
    std::string process_command(const std::string& cmd);

    tcp::socket socket_;
    Database& db_;
    char data_[1024];
};

class Server {
public:
    Server(boost::asio::io_context& io_context, short port);

private:
    void do_accept();

    tcp::acceptor acceptor_;
    Database db_;
};