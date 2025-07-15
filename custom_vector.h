// custom_vector.h
#pragma once

#include <memory>
#include <utility>
#include <stdexcept>

template<typename T, class Allocator = std::allocator<T>>
class CustomVector {
public:
    class Iterator {
    public:
        Iterator(T* ptr) : ptr_(ptr) {}

        Iterator& operator++() {
            ++ptr_;
            return *this;
        }

        bool operator==(const Iterator& other) const {
            return ptr_ == other.ptr_;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

        T& operator*() const {
            return *ptr_;
        }

    private:
        T* ptr_;
    };

    CustomVector() {
        if constexpr (has_max_size<Allocator>::value) {
            // Для аллокаторов с ограниченным размером
            capacity_ = allocator_.max_size;
            data_ = allocator_.allocate(capacity_);
        } else {
            // Для стандартных аллокаторов
            capacity_ = 16;
            data_ = allocator_.allocate(capacity_);
        }
    }

    ~CustomVector() {
        for (size_t i = 0; i < size_; ++i) {
            allocator_.destroy(data_ + i);
        }
        allocator_.deallocate(data_, capacity_);
    }

    void push_back(const T& value) {
        if (size_ >= capacity_) {
            throw std::bad_alloc(); // Не можем расшириться
        }
        allocator_.construct(data_ + size_, value);
        ++size_;
    }

    Iterator begin() const {
        return Iterator(data_);
    }

    Iterator end() const {
        return Iterator(data_ + size_);
    }

private:
    template<typename A>
    struct has_max_size {
    private:
        template<typename U>
        static auto test(int) -> decltype(std::declval<U>().max_size, std::true_type{});
        
        template<typename>
        static std::false_type test(...);
        
    public:
        static constexpr bool value = decltype(test<A>(0))::value;
    };

    T* data_{nullptr};
    size_t size_{0};
    size_t capacity_{0};
    Allocator allocator_;
};