# Create the test executable
add_executable(spsc_queue_test test_spsc_queue.cpp)

# Link GTest, the threading library (pthread), and include our source headers
target_link_libraries(spsc_queue_test gtest_main gtest pthread)
target_include_directories(spsc_queue_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Register the test with CTest
add_test(NAME SPSCQueueStressTest COMMAND spsc_queue_test)