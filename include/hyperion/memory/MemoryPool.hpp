// 1. Include Guards or #pragma once to prevent double inclusion
#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <iostream> // Necessary dependencies
#include <vector>

// 2. Class Declaration
template <typename T>
class MemoryPool {
public:
    MemoryPool(T value);
    T* allocate(size_t size);
    int allocate(T* slot);
    void display() const;
private:
    T data;
    std::vector<T*> free_list;

};

// 3. Method Definitions (must stay in the header)
template <typename T>
MemoryPool<T>::MemoryPool(T value) : data(value) {}

template <typename T>
void MemoryPool<T>::display() const {
    std::cout << "Value: " << data << std::endl;
}

template <typename T>
T* MemoryPool<T>::allocate(size_t size) {
    std::cout <<  size << std::endl;
    return &data;
}

template <typename T>
int MemoryPool<T>::allocate(T* slot) {
    data = slot;
    return 0;
}

#endif
