// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_SUPPORT_ARENA_H
#define INRCC_SUPPORT_ARENA_H

/// @file Support/Arena.h
/// @brief Provides an arena allocator.

#include <inr/Support/Align.h>
#include <inr/Support/Compiler.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <utility>

namespace inrcc {

class Arena {
    constexpr static size_t MINIMUM_ALLOCATION = 0x10000;

    uint8_t* cur_;
    uint8_t* end_;
    void* head_;

    void grow(size_t minimum) {
        size_t size = (minimum > MINIMUM_ALLOCATION) ? minimum + sizeof(void*)
                                                     : MINIMUM_ALLOCATION;
        void* newBlock = malloc(size);

        *(void**)newBlock = head_;
        head_ = newBlock;

        cur_ = (uint8_t*)newBlock + sizeof(void*);
        end_ = (uint8_t*)newBlock + size;
    }

    void freeall() noexcept {
        while(head_) {
            void* prev = *(void**)head_;
            free(head_);
            head_ = prev;
        }
    }

public:
    Arena() : cur_(nullptr), end_(nullptr), head_(nullptr) {
        grow(0);
    }

    ~Arena() noexcept {
        freeall();
    }

    void* allocate(size_t size, size_t align) {
        uintptr_t addr = (uintptr_t)cur_;
        uintptr_t aligned = (addr + align - 1) & ~(align - 1);

        if(aligned + size <= (uintptr_t)end_) {
            cur_ = (uint8_t*)(aligned + size);
            return (void*)aligned;
        }

        grow(size + align);
        return allocate(size, align);
    }

    template<typename T, typename... Args>
    T* alloc(Args&&... args) {
        void* mem = allocate(sizeof(T), alignof(T));
        return new(mem) T(std::forward<Args>(args)...);
    }

    template<typename T>
    T* allocN(size_t n) {
        void* mem = allocate(sizeof(T) * n, alignof(T));
        return new(mem) T[n];
    }

    /// @brief Risky function, assumes the pointer is from this arena.
    template<typename T>
    void tryFree(T* ptr) {
        ptr->~T();
    }

    Arena(Arena&& other) noexcept :
        cur_(other.cur_), end_(other.end_), head_(other.head_) {
        other.cur_ = other.end_ = nullptr;
        other.head_ = nullptr;
    }

    Arena& operator=(Arena&& other) noexcept {
        if(this != &other) {
            freeall();

            cur_ = other.cur_;
            end_ = other.end_;
            head_ = other.head_;

            other.cur_ = other.end_ = nullptr;
            other.head_ = nullptr;
        }
        return *this;
    }

    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;
};

} // namespace inrcc

#endif // INRCC_SUPPORT_ARENA_H
