#pragma once

#include "OrderBook.hpp"
#include "Order.hpp"
#include <algorithm>

namespace hyperion::matching {

class Engine {
private:
    // The Engine holds a reference to the physical Order Book
    OrderBook& book_;

public:
    explicit Engine(OrderBook& book) : book_(book) {}

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    void process_order(Order* order);
};

} // namespace hyperion::matching
