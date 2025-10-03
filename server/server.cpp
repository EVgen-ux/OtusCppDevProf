#include "server.hpp"
#include <iostream>
#include <sstream>

// Конструктор Server
Server::Server(boost::asio::io_context& io_context, short port) 
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    do_accept();
}

// Реализация do_accept() ДОЛЖНА быть здесь
void Server::do_accept() {
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::make_shared<Session>(std::move(socket), db_)->start();
        }
        do_accept();
    });
}

// Конструктор Session
Session::Session(tcp::socket socket, Database& db) : socket_(std::move(socket)), db_(db) {}

void Session::start() {
    std::cout << "New client connected" << std::endl;
    do_read();
}

void Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, 1024),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                std::string cmd(data_, length);
                std::string response = process_command(cmd);
                do_write(response);
            } else {
                std::cout << "Client disconnected" << std::endl;
            }
        });
}

void Session::do_write(const std::string& response) {
    auto self(shared_from_this());
    std::string response_with_newline = response;
    boost::asio::async_write(socket_, boost::asio::buffer(response_with_newline),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                do_read();
            }
        });
}

std::string Session::process_command(const std::string& cmd) {
    std::string clean_cmd = cmd;
    clean_cmd.erase(clean_cmd.find_last_not_of(" \n\r\t") + 1);
    std::cout << "Received command: " << clean_cmd << std::endl;
    
    std::istringstream iss(clean_cmd);
    std::string action, table, name;
    int id;
    
    iss >> action;
    
    if (action == "INSERT") {
        if (iss >> table >> id) {
            std::getline(iss, name);
            name.erase(0, name.find_first_not_of(" "));
            
            if (table != "A" && table != "B") {
                return "ERR: Table must be 'A' or 'B'";
            }
            if (name.empty()) {
                return "ERR: Name cannot be empty";
            }
            if (id < 0) {
                return "ERR: ID must be non-negative";
            }
            
            std::string result = db_.insert(table, id, name);
            std::cout << "Sending response: " << result << std::endl;
            return result;
        }
        return "ERR: Invalid INSERT format. Use: INSERT <A|B> <id> <name>";
    }
    else if (action == "TRUNCATE") {
        if (iss >> table) {
            if (table != "A" && table != "B") {
                return "ERR: Table must be 'A' or 'B'";
            }
            std::string result = db_.truncate(table);
            std::cout << "Sending response: " << result << std::endl;
            return result;
        }
        return "ERR: Invalid TRUNCATE format. Use: TRUNCATE <A|B>";
    }
    else if (action == "INTERSECTION") {
        if (iss >> table) {
            return "ERR: INTERSECTION takes no parameters";
        }
        std::string result = db_.intersection();
        std::cout << "Sending response: " << (result == "OK" ? "OK" : "multi-line data") << std::endl;
        return result;
    }
    else if (action == "SYMMETRIC_DIFFERENCE") {
        if (iss >> table) {
            return "ERR: SYMMETRIC_DIFFERENCE takes no parameters";
        }
        std::string result = db_.symmetric_difference();
        std::cout << "Sending response: " << (result == "OK" ? "OK" : "multi-line data") << std::endl;
        return result;
    }
    
    return "ERR: Unknown command. Available: INSERT, TRUNCATE, INTERSECTION, SYMMETRIC_DIFFERENCE";
}