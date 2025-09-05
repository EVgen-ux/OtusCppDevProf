#include <iostream>
#include <memory>
#include <thread>
#include <boost/asio.hpp>
#include "../include/async_bulk/async.h"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, size_t bulk_size)
        : socket_(std::move(socket)), bulk_size_(bulk_size) {
        context_id_ = AsyncBulk::connect(bulk_size_);
    }

    ~Session() {
        AsyncBulk::disconnect(context_id_);
    }

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        boost::asio::async_read_until(socket_, buffer_, '\n',
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::istream is(&buffer_);
                    std::string command;
                    std::getline(is, command);
                    
                    if (!command.empty() && command.back() == '\r') {
                        command.pop_back();
                    }
                    
                    AsyncBulk::receive(context_id_, command);
                    do_read();
                } else if (ec != boost::asio::error::eof) {
                    std::cerr << "Read error: " << ec.message() << std::endl;
                }
            });
    }

    tcp::socket socket_;
    boost::asio::streambuf buffer_;
    size_t bulk_size_;
    AsyncBulk::ContextID context_id_;
};

class Server {
public:
    Server(boost::asio::io_context& io_context, short port, size_t bulk_size)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), bulk_size_(bulk_size) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket), bulk_size_)->start();
                } else {
                    std::cerr << "Accept error: " << ec.message() << std::endl;
                }
                do_accept();
            });
    }

    tcp::acceptor acceptor_;
    size_t bulk_size_;
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: bulk_server <port> <bulk_size>" << std::endl;
        return 1;
    }

    try {
        short port = std::stoi(argv[1]);
        size_t bulk_size = std::stoul(argv[2]);

        boost::asio::io_context io_context;
        Server server(io_context, port, bulk_size);

        std::cout << "Server started on port " << port 
                  << " with bulk size " << bulk_size << std::endl;
        
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}