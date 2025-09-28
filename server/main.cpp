#include <iostream>
#include <memory>
#include <thread>
#include <csignal>
#include <boost/asio.hpp>
#include "../include/async_bulk/async.h"

using boost::asio::ip::tcp;

std::atomic<bool> shutdown_requested{false};
boost::asio::io_context* io_context_ptr = nullptr;

void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    shutdown_requested.store(true);
    if (io_context_ptr) {
        io_context_ptr->stop();
    }
}

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, AsyncBulk::ContextID context_id)
        : socket_(std::move(socket)), context_id_(context_id) {
    }

    ~Session() {
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
                } else {
                    if (ec != boost::asio::error::eof) {
                        std::cerr << "Read error: " << ec.message() << std::endl;
                    }
                }
            });
    }

    tcp::socket socket_;
    boost::asio::streambuf buffer_;
    AsyncBulk::ContextID context_id_;
};

class Server {
public:
    Server(boost::asio::io_context& io_context, short port, size_t bulk_size)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), bulk_size_(bulk_size) {
        // Создаем ОДИН общий контекст для всех соединений
        context_id_ = AsyncBulk::connect(bulk_size_);
        do_accept();
    }

    ~Server() {
        // Финализируем все оставшиеся команды
        std::cout << "Finalizing remaining commands..." << std::endl;
        AsyncBulk::disconnect(context_id_);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

private:
    void do_accept() {
        if (shutdown_requested.load()) {
            return;
        }
        
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket), context_id_)->start();
                } else {
                    if (!shutdown_requested.load()) {
                        std::cerr << "Accept error: " << ec.message() << std::endl;
                    }
                }
                
                if (!shutdown_requested.load()) {
                    do_accept();
                }
            });
    }

    tcp::acceptor acceptor_;
    size_t bulk_size_;
    AsyncBulk::ContextID context_id_;
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
        io_context_ptr = &io_context;
        
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](boost::system::error_code, int signal) {
            signal_handler(signal);
        });

        Server server(io_context, port, bulk_size);

        std::cout << "Server started on port " << port 
                  << " with bulk size " << bulk_size << std::endl;
        std::cout << "Press Ctrl+C to stop the server..." << std::endl;
        
        io_context.run();
        
        std::cout << "Server stopped. Waiting for async operations to complete..." << std::endl;
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        AsyncBulk::shutdown();
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}