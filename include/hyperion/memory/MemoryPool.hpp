#pragma once

#include <vector>
#include <cstddef>
#include <stdexcept>
#include <cassert>
#include <iostream> // Needed for std::cout

namespace hyperion::memory {

template <typename T>
class MemoryPool {
private:
    std::vector<T> store_;
    std::vector<T*> free_list_;

public:
    // Constructor using a Member Initialization List for max efficiency
    explicit MemoryPool(size_t capacity) : store_(capacity) {
        // Reserve memory for the pointers so push_back NEVER triggers an OS reallocation
        free_list_.reserve(capacity); 

        // Populate the free list with the addresses of our pre-allocated objects
        for (T& pos : store_) {
            free_list_.push_back(&pos);
        }
    }

    // Prevent copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    // $O(1)$ Allocation
    T* allocate() {
        if (free_list_.empty()) {
            throw std::bad_alloc();
        }
        
        T* free_slot = free_list_.back();
        free_list_.pop_back();
        return free_slot;
    }

    // $O(1)$ Deallocation
    void deallocate(T* ptr) {
        if (ptr == nullptr) return;

        // Safety check: Does this pointer actually belong to our contiguous block?
        T* store_start = store_.data();
        T* store_end = store_start + store_.size();

        if (ptr < store_start || ptr >= store_end) {
            std::cerr << "[MEMORY POOL FATAL]: Bad deallocation, pointer out of range." << std::endl;
            assert(false);
        }

        free_list_.push_back(ptr);
    }

    // Utility
    size_t available() const {
        return free_list_.size();
    }
    
    size_t capacity() const {
        return store_.size();
    }
};

} // namespace hyperion::memory