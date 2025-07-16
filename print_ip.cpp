#include <algorithm>
#include <cstdint>
#include <iostream>
#include <list>
#include <string>
#include <tuple>
#include <vector>
#include <utility> 

//  int

template<typename T, 
         std::enable_if_t<std::is_integral_v<T>, bool> = true>
void print_ip(T val) {
    uint8_t* p = reinterpret_cast<uint8_t*>(&val);
    
    std::cout << std::to_string(p[sizeof(val) - 1]);

    for (long long i = sizeof(val) - 2; i >= 0; --i) {
        std::cout << "." << std::to_string(p[i]);
    }

    std::cout << std::endl;
}

//  string

template<typename T>
struct is_string : std::false_type {};

template<>
struct is_string<std::string> : std::true_type {};

template<typename T,
         std::enable_if_t<is_string<T>::value, bool> = true>
void print_ip(T val) {
    std::cout << val << std::endl;
}

//  vector, list

template<typename T>
struct is_container : std::false_type {};

template<typename T, typename Alloc>
struct is_container<std::vector<T, Alloc>> : std::true_type {};

template<typename T, typename Alloc>
struct is_container<std::list<T, Alloc>> : std::true_type {};

template<typename T>
constexpr bool is_container_v = is_container<T>::value;

template<typename T, 
         std::enable_if_t<is_container_v<T>, bool> = true>
void print_ip(T val) {
    std::cout << *val.begin();

    for (auto it = std::next(val.begin()); it != val.end(); ++it) {
    std::cout<< "." << *it ;
}

    std::cout << std::endl;
}

//  tuple

template <typename... T> struct is_tuple : std::false_type {};
template <typename... T> struct is_tuple<std::tuple<T...>> : std::true_type {};

template<typename Tuple>
struct all_same_types;

template<typename Head, typename... Tail>
struct all_same_types<std::tuple<Head, Tail...>> {
    static constexpr bool value = (std::is_same_v<Head, Tail> && ...);
};

template <typename TupleT, std::size_t... Is>
void print_tuple_impl(const TupleT& tp, std::index_sequence<Is...>) {
    size_t index = 0;
    const char* sep = "";
    ((std::cout << sep << std::get<Is>(tp), sep = ".", index++), ...);
}

template<typename TupleT, std::enable_if_t<is_tuple<TupleT>::value, bool> = true>
void print_ip(const TupleT& tp) {
    static_assert(all_same_types<TupleT>::value, "All tuple elements must have the same type");
    print_tuple_impl(tp, std::make_index_sequence<std::tuple_size_v<TupleT>>{});
    std::cout << std::endl;
}

int main() {
    print_ip( int8_t{-1} ); // 255
    print_ip( int16_t{0} ); // 0.0
    print_ip( int32_t{2130706433} ); // 127.0.0.1
    print_ip( int64_t{8875824491850138409} );// 123.45.67.89.101.112.131.41
    print_ip( std::string{"Hello, World!"} ); // Hello, World!
    print_ip( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400
    print_ip( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100
    print_ip( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0
    // print_ip( std::make_tuple('A', 456, 789, 0) ); // Error!!!
}