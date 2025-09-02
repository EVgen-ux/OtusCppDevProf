#include <async_bulk/async.h>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

void demonstrate_basic_usage() {
    std::cout << " Демонстрация базового использования" << std::endl;
    
    auto ctx = AsyncBulk::connect(3);
    std::cout << "Создан контекст: " << ctx << std::endl;
    
    AsyncBulk::receive(ctx, "command_1");
    AsyncBulk::receive(ctx, "command_2");
    AsyncBulk::receive(ctx, "command_3");
    
    AsyncBulk::receive(ctx, "{");
    AsyncBulk::receive(ctx, "dynamic_cmd_1");
    AsyncBulk::receive(ctx, "dynamic_cmd_2");
    AsyncBulk::receive(ctx, "}");
    
    AsyncBulk::disconnect(ctx);
}

void demonstrate_multithreading() {
    std::cout << "\n";
    std::cout << " Демонстрация многопоточности" << std::endl;
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back([i]() {
            auto ctx = AsyncBulk::connect(2 + i);
            
            for (int j = 0; j < 4; ++j) {
                std::string cmd = "thread" + std::to_string(i) + "_cmd" + std::to_string(j);
                AsyncBulk::receive(ctx, cmd);
            }
            
            AsyncBulk::disconnect(ctx);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}

int main() {
    std::cout << " Демонстрационное приложение" << std::endl;
    std::cout << "\n";
    
    demonstrate_basic_usage();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    demonstrate_multithreading();

    std::cout << "Ожидание завершения асинхронных операций..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    AsyncBulk::shutdown();

    std::cout << "\n";
    std::cout << "Все демонстрации завершены успешно!" << std::endl;
    
    return 0;
}