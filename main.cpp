#include <iostream>
#include <map>

#include "custom_allocator.h"
#include "custom_vector.h"

constexpr int N{10};

template <typename Key, typename Value, typename Allocator>
void fill_map() {
    std::map<Key, Value, std::less<>, Allocator> mp{};

    for (int i = 0, f = 1; i < N; i++, f *= i) {
        mp.insert({i, f});
    }
    
    for (auto [key, value] : mp) {
        std::cout << '{' << key << ", " << value << "} ";
    }
    std::cout << std::endl;
}

template <typename Allocator>
void fill_custom_vector() {
    CustomVector<int, Allocator> list;
    for (int i = 0; i < N; i++) {
        list.push_back(i);
    }

    for (auto value : list) {
        std::cout << value << ' ';
    }

    std::cout << std::endl;
}

int main() {
    std::cout << "map with default allocator ";
    fill_map<int, int, std::allocator<std::pair<const int, int>>>();

    std::cout << "map with custom allocator ";
    fill_map<int, int, CustomAllocator<std::pair<const int, int>, N>>();

    std::cout << "custom container ";
    fill_custom_vector<std::allocator<int>>();

    std::cout << "custom container with custom allocator ";
    fill_custom_vector<CustomAllocator<int, N>>();

    return 0;
}
