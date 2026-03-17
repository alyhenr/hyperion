#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>
#include "PriceLevel.hpp"
#include "Order.hpp"

namespace hyperion::matching {

class OrderBook {
private:
    std::vector<PriceLevel> bids_;
    std::vector<PriceLevel> asks_;
    std::vector<Order*> order_map_;

    uint64_t best_bid_price_{0};
    uint64_t best_ask_price_{UINT64_MAX};

    uint64_t min_price_;
    uint64_t max_price_;
    uint64_t tick_size_;

    inline size_t price_to_index(uint64_t price) const {
        return (price - min_price_) / tick_size_;
    }

public:
    OrderBook(uint64_t min_price, uint64_t max_price, uint64_t tick_size, size_t max_orders) 
        : min_price_(min_price), max_price_(max_price), tick_size_(tick_size) {
        
        // 1. Safety Check
        if (min_price >= max_price || tick_size == 0) {
            throw std::invalid_argument("Invalid price boundaries or tick size.");
        }

        // 2. Calculate size of price level vectorss
        size_t n_pl = ((max_price_ - min_price_) / tick_size_) + 1;

        // 3. Resize arrays
        bids_.resize(n_pl);
        asks_.resize(n_pl);

        // 4. Initialize prices based STRICTLY on the price_to_index offset
        for (size_t i = 0; i < n_pl; ++i) {
            uint64_t current_price = min_price_ + (i * tick_size_);
            
            bids_[i].price = current_price;
            asks_[i].price = current_price;
        }

        // 5. Resize Order Map
        order_map_.resize(max_orders, nullptr);
    }

    std::string to_string() const {
        return "---Order Book---\nMin. price: " + std::to_string(min_price_) + 
               "\nMax. price: " + std::to_string(max_price_) + 
               "\nTick size: " + std::to_string(tick_size_);
    }
    
    // TODO:
    void add_order(Order* order);
    void cancel_order(uint64_t order_id);
};

} // namespace hyperion::matching