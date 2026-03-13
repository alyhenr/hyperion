#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <chrono>
#include "hyperion/utils/SPSCQueue.hpp"

using namespace hyperion::utils;

// --- Sanity Check: Single Threaded ---
TEST(SPSCQueueTest, BasicPushPop) {
    SPSCQueue<int> queue(2);
    
    EXPECT_TRUE(queue.push(42));
    EXPECT_TRUE(queue.push(99));
    EXPECT_FALSE(queue.push(100)); // Queue should be full

    int val = 0;
    EXPECT_TRUE(queue.pop(val));
    EXPECT_EQ(val, 42); // Must be strictly FIFO

    EXPECT_TRUE(queue.pop(val));
    EXPECT_EQ(val, 99);

    EXPECT_FALSE(queue.pop(val)); // Queue should be empty
}

// --- Multi-Threaded Stress Test ---
TEST(SPSCQueueTest, ConcurrentProducerConsumer) {
    const size_t QUEUE_CAPACITY = 1024;
    const int MESSAGES_TO_SEND = 1'000'000;
    
    SPSCQueue<int> queue(QUEUE_CAPACITY);

    // This thread will push numbers 1 to MESSAGES_TO_SEND
    std::thread producer([&]() {
        for (int i = 1; i <= MESSAGES_TO_SEND; ++i) {
            while(!queue.push(i)) std::this_thread::yield();
        }
    });

    // This thread will read numbers and ensure they are perfectly sequential
    std::thread consumer([&]() {
        int expected_value = 1;
        
        while (expected_value <= MESSAGES_TO_SEND) {
            int received_value = 0;

            while(!queue.pop(received_value)) std::this_thread::yield();

            ASSERT_EQ(received_value, expected_value);
            ++expected_value;
        }
    });

    // Wait for both threads to finish their massive workload
    producer.join();
    consumer.join();
}
