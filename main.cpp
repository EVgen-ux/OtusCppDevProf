#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>

// Cтроку IP на tuple<int, int, int, int>
std::tuple<int, int, int, int> split(const std::string& ip_str, char d) {
    size_t start = 0;
    size_t stop = ip_str.find(d);
    int part1 = std::stoi(ip_str.substr(start, stop - start));

    start = stop + 1;
    stop = ip_str.find(d, start);
    int part2 = std::stoi(ip_str.substr(start, stop - start));

    start = stop + 1;
    stop = ip_str.find(d, start);
    int part3 = std::stoi(ip_str.substr(start, stop - start));

    start = stop + 1;
    int part4 = std::stoi(ip_str.substr(start));

    return {part1, part2, part3, part4};
}

// Вывод IP из tuple
void print_ip(const std::tuple<int, int, int, int>& ip) {
    auto [a, b, c, d] = ip;
    std::cout << a << "." << b << "." << c << "." << d << std::endl;
}

// Сортировка в обратном порядке
void revers_sort( std::vector<std::tuple<int, int, int, int>>& ip_pool) {
    std::sort(ip_pool.rbegin(), ip_pool.rend());
    for (const auto& ip : ip_pool) {
        print_ip(ip);
    }
}


// Фильтрация по первому байту
void filter_by_first_byte(const std::vector<std::tuple<int, int, int, int>>& ip_pool, int first) {
    for (const auto& ip : ip_pool) {
        if (std::get<0>(ip) == first) {
            print_ip(ip);
        }
    }
}

// Фильтрация по первому и второму байтам
void filter_by_first_and_second(const std::vector<std::tuple<int, int, int, int>>& ip_pool, int first, int second) {
    for (const auto& ip : ip_pool) {
        if (std::get<0>(ip) == first && std::get<1>(ip) == second) {
            print_ip(ip);
        }
    }
}

// Фильтрация по указанному байту
void filter_by_any_byte(const std::vector<std::tuple<int, int, int, int>>& ip_pool, int byte) {
    for (const auto& ip : ip_pool) {
        if (std::get<0>(ip) == byte || std::get<1>(ip) == byte || std::get<2>(ip) == byte || std::get<3>(ip) == byte) {
            print_ip(ip);
        }
    }
}

int main() {
    try {
        std::vector<std::tuple<int, int, int, int>> ip_pool;

        // Чтение из потока
        for (std::string line; std::getline(std::cin, line);) {
            size_t tab_pos = line.find('\t');
            std::string ip_str = line.substr(0, tab_pos);
            ip_pool.push_back(split(ip_str,'.'));
        }

        revers_sort(ip_pool);

        filter_by_first_byte(ip_pool, 1);

        filter_by_first_and_second(ip_pool, 46, 70);

        filter_by_any_byte(ip_pool, 46);
    }

    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}