#pragma once

#include <vector>
#include <atomic>
#include <cstddef>

namespace hyperion::utils {

template <typename T>
class SPSCQueue {
private:
    std::vector<T> buffer_;
    const size_t capacity_;

    alignas(64) std::atomic<size_t> write_index_{0};
    alignas(64) std::atomic<size_t> read_index_{0};

public:
    explicit SPSCQueue(size_t capacity) : capacity_(capacity) {
        buffer_.resize(capacity_ + 1);
    }

    SPSCQueue(const SPSCQueue&) = delete;
    SPSCQueue& operator=(const SPSCQueue&) = delete;

    bool push(const T& item) {
        // 1. Get current and next write indices
        size_t current_w = write_index_.load(std::memory_order_relaxed);
        size_t next_w = (current_w + 1) % buffer_.size();
        
        // 2. Read the consumer's index
        size_t current_r = read_index_.load(std::memory_order_acquire);
        
        // 3. Check if full
        if (next_w == current_r) return false; 
        
        // 4. Write to the CURRENT slot
        buffer_[current_w] = item;
        
        // 5. Publish the NEXT slot to the consumer
        write_index_.store(next_w, std::memory_order_release);
        
        return true; 
    }

    bool pop(T& out_item) {
        // 1. Get current read index
        size_t current_r = read_index_.load(std::memory_order_relaxed);
        
        // 2. Read the producer's index
        size_t current_w = write_index_.load(std::memory_order_acquire);
        
        // 3. Check if empty
        if (current_r == current_w) return false; 
        
        // 4. Read from the CURRENT slot
        out_item = buffer_[current_r];
        
        // 5. Calculate NEXT read index and publish
        size_t next_r = (current_r + 1) % buffer_.size();
        read_index_.store(next_r, std::memory_order_release);       
        
        return true; 
    }
};

} // namespace hyperion::utils