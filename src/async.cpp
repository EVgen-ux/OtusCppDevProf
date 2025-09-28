#include "../include/async_bulk/async.h"
#include "../include/async_bulk/bulk.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>
#include <queue>
#include <condition_variable>
#include <random>
#include <chrono>

namespace AsyncBulk {

class AsyncProcessor {
public:
    AsyncProcessor() : running_(true) {
        console_thread_ = std::thread(&AsyncProcessor::consoleWorker, this);
        for (int i = 0; i < 4; ++i) {
            file_threads_.emplace_back(&AsyncProcessor::fileWorker, this);
        }
    }
    
    ~AsyncProcessor() {
        running_ = false;
        console_cv_.notify_all();
        file_cv_.notify_all();
        
        if (console_thread_.joinable()) console_thread_.join();
        for (auto& t : file_threads_) {
            if (t.joinable()) t.join();
        }
    }
    
    ContextID createContext(size_t block_size) {
        std::lock_guard<std::mutex> lock(contexts_mutex_);
        ContextID id = next_context_id_++;
        contexts_[id] = std::make_unique<Bulk>(block_size);
        return id;
    }
    
    void processCommand(ContextID id, const std::string& command) {
        std::lock_guard<std::mutex> lock(contexts_mutex_);
        if (contexts_.count(id)) {
            contexts_[id]->processCommand(command);
            
            auto output = contexts_[id]->getOutput();
            if (!output.empty()) {

                std::pair<ContextID, std::vector<std::string>> context_output{id, output};
                
                {
                    std::lock_guard<std::mutex> console_lock(console_mutex_);
                    console_queue_.push(context_output);
                }
                console_cv_.notify_one();
                
                {
                    std::lock_guard<std::mutex> file_lock(file_mutex_);
                    file_queue_.push(context_output);
                }
                file_cv_.notify_one();
            }
        }
    }
    
    void destroyContext(ContextID id) {
        std::lock_guard<std::mutex> lock(contexts_mutex_);
        if (contexts_.count(id)) {
            contexts_[id]->finalize();
            
            auto output = contexts_[id]->getOutput();
            if (!output.empty()) {
                std::pair<ContextID, std::vector<std::string>> context_output{id, output};
                
                {
                    std::lock_guard<std::mutex> console_lock(console_mutex_);
                    console_queue_.push(context_output);
                }
                console_cv_.notify_one();
                
                {
                    std::lock_guard<std::mutex> file_lock(file_mutex_);
                    file_queue_.push(context_output);
                }
                file_cv_.notify_one();
            }
            
            contexts_.erase(id);
        }
    }

private:
    void consoleWorker() {
        while (running_) {
            std::unique_lock<std::mutex> lock(console_mutex_);
            console_cv_.wait(lock, [this]() { 
                return !console_queue_.empty() || !running_; 
            });
            
            if (!console_queue_.empty()) {
                auto context_output = console_queue_.front();
                console_queue_.pop();
                lock.unlock();
                
                auto& bulk = context_output.second;
                std::cout << "bulk: ";
                for (size_t i = 0; i < bulk.size(); ++i) {
                    if (i != 0) std::cout << ", ";
                    std::cout << bulk[i];
                }
                std::cout << std::endl;
            }
        }
    }
    
    void fileWorker() {
        static std::atomic<size_t> file_counter{0};
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1000, 9999);
        
        while (running_) {
            std::unique_lock<std::mutex> lock(file_mutex_);
            file_cv_.wait(lock, [this]() { 
                return !file_queue_.empty() || !running_; 
            });
            
            if (!file_queue_.empty()) {
                auto context_output = file_queue_.front();
                file_queue_.pop();
                lock.unlock();
                
                auto& bulk = context_output.second;
                auto context_id = context_output.first;
                
                auto timestamp = std::chrono::system_clock::now();
                auto time_t = std::chrono::system_clock::to_time_t(timestamp);
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    timestamp.time_since_epoch()) % 1000;
                
                 std::string filename = "bulk" + std::to_string(context_id) + "_" + 
                                      std::to_string(time_t) + "_" + 
                                      std::to_string(ms.count()) + "_" + 
                                      std::to_string(dis(gen)) + ".log";
                
                std::ofstream file(filename);
                if (file.is_open()) {
                    file << "bulk: ";
                    for (size_t i = 0; i < bulk.size(); ++i) {
                        if (i != 0) file << ", ";
                        file << bulk[i];
                    }
                    file << std::endl;
                    file.close();
                }
            }
        }
    }

    std::unordered_map<ContextID, std::unique_ptr<Bulk>> contexts_;
    std::mutex contexts_mutex_;
    std::atomic<ContextID> next_context_id_{1};
    
    std::queue<std::pair<ContextID, std::vector<std::string>>> console_queue_;
    std::mutex console_mutex_;
    std::condition_variable console_cv_;
    
    std::queue<std::pair<ContextID, std::vector<std::string>>> file_queue_;
    std::mutex file_mutex_;
    std::condition_variable file_cv_;
    
    std::thread console_thread_;
    std::vector<std::thread> file_threads_;
    std::atomic<bool> running_;
};

static AsyncProcessor processor;

ContextID connect(size_t block_size) {
    return processor.createContext(block_size);
}

void receive(ContextID context_id, const std::string& command) {
    processor.processCommand(context_id, command);
}

void disconnect(ContextID context_id) {
    processor.destroyContext(context_id);
}

void shutdown() {
}


} // namespace AsyncBulk