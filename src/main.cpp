#include <iostream>
#include <thread>
#include <atomic>
#include <csignal>

// TODO: Include custom headers once ready
#include "../include/hyperion/memory/MemoryPool.hpp"
// #include "utils/SPSCQueue.hpp"
// #include "matching/Engine.hpp"
// #include "network/Ingress.hpp"
// #include "network/Egress.hpp"

// Global flag for graceful shutdown
std::atomic<bool> g_running{true};

void signal_handler(int signum) {
    std::cout << "\n[Orchestrator] Interrupt signal (" << signum << ") received. Initiating graceful shutdown...\n";
    g_running.store(false, std::memory_order_release);
}

// A dummy struct simulating a network order
struct Order {
    uint64_t order_id;
    double price;
    uint32_t quantity;
    char side;
};

void run_pool_test() {
    std::cout << "--- Testing Memory Pool ---\n";
    const size_t POOL_SIZE = 1'000'000;;
    
    hyperion::memory::MemoryPool<Order> pool(POOL_SIZE);
    std::cout << "Pool created with capacity: " << pool.capacity() << "\n";

    // 1. Time the allocation of 1 million orders
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<Order*> active_orders;
    active_orders.reserve(POOL_SIZE);

    for (size_t i = 0; i < POOL_SIZE; ++i) {
        Order* order = pool.allocate();
        order->order_id = i;
        active_orders.push_back(order);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "Allocated " << POOL_SIZE << " objects in " << duration_ms << " ms.\n";
    std::cout << "Available slots: " << pool.available() << "\n";

    // 2. Time the deallocation
    start = std::chrono::high_resolution_clock::now();
    
    for (Order* order : active_orders) {
        pool.deallocate(order);
    }

    end = std::chrono::high_resolution_clock::now();
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Deallocated " << POOL_SIZE << " objects in " << duration_ms << " ms.\n";
    std::cout << "Available slots: " << pool.available() << "\n";
}

// TODO: Implement a utility function to pin a thread to a specific CPU core
// void pin_thread_to_core(std::thread& t, int core_id) {
//     // Hint: Look up pthread_setaffinity_np for Linux
// }

int main() {
    // 1. Setup signal handling for safe shutdowns (Ctrl+C)
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::cout << "[Orchestrator] Booting Hyperion Matching Engine...\n";

    // 2. Memory Pre-allocation (The "Zero Runtime Allocation" rule)
    std::cout << "[Orchestrator] Allocating global memory pools...\n";
    // TODO: Initialize your massive, contiguous Object Pools for Orders and Trades here.

    // 3. Initialize Inter-Thread Communication (Lock-Free Queues)
    // TODO: Instantiate SPSC (Single-Producer Single-Consumer) ring buffers.
    // e.g., auto inbound_queue = SPSCQueue<Order>(1024 * 1024); 
    // e.g., auto outbound_queue = SPSCQueue<ExecutionReport>(1024 * 1024);

    // 4. Initialize Core Components
    // TODO: Instantiate the Matching Engine, passing references to the queues.
    // TODO: Instantiate Ingress (TCP/UDP listener) and Egress (Market Data publisher).

    // 5. Thread Launch & CPU Core Pinning
    std::cout << "[Orchestrator] Igniting threads and pinning to cores...\n";
    
    // Launch Egress (Network TX)
    // std::thread egress_thread([/* refs to queues */]() {
    //     // TODO: egress.run(g_running);
    // });
    // pin_thread_to_core(egress_thread, 2); 

    // // Launch Ingress (Network RX)
    // std::thread ingress_thread([/* refs to queues */]() {
    //     // TODO: ingress.run(g_running);
    // });
    // pin_thread_to_core(ingress_thread, 3);

    // // Launch the Matching Core (The critical path)
    // std::thread matching_thread([/* refs to queues and LOB */]() {
    //     // TODO: engine.run(g_running); 
    //     // Inside this run loop, there should be ZERO allocations, ZERO locks, and ZERO system calls.
    // });
    // // The matching core gets the most isolated, undisturbed CPU core you have.
    // pin_thread_to_core(matching_thread, 4); 

    // std::cout << "[Orchestrator] Hyperion is live. Awaiting orders.\n";
    run_pool_test();
    // // 6. Main thread waits for shutdown signal
    while (g_running.load(std::memory_order_acquire)) {
        // Main thread can handle low-priority telemetry, logging, or just sleep.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // // 7. Teardown
    // std::cout << "[Orchestrator] Joining threads and releasing resources...\n";
    
    // // TODO: Ensure queues are flushed and threads exit their loops cleanly.
    // if (ingress_thread.joinable()) ingress_thread.join();
    // if (matching_thread.joinable()) matching_thread.join();
    // if (egress_thread.joinable()) egress_thread.join();

    std::cout << "[Orchestrator] Shutdown complete.\n";
    return 0;
}