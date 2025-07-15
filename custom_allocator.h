#pragma once

#include <stdexcept> 
#include <cstddef>
#include <utility>

template <typename T, size_t R>
struct CustomAllocator {
    using value_type = T;
    static constexpr size_t max_size = R;

    template <typename U>
    struct rebind {
        using other = CustomAllocator<U, R>;
    };

    CustomAllocator() = default;
    ~CustomAllocator() = default;

    template <typename U>
    explicit CustomAllocator(const CustomAllocator<U, R>&) {}

    T* allocate(std::size_t n) {
        if (n + allocated_count_ > max_size) {
            throw std::bad_alloc();
        }

        auto p = &storage[allocated_count_ * sizeof(T)];
        allocated_count_ += n;
        return reinterpret_cast<T*>(p);
    }

    void deallocate(T*, std::size_t) {}

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        new (p) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U* p) {
        p->~U();
    }

    size_t allocated_count_{0u};
    std::byte storage[R * sizeof(T)];
};