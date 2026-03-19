// src/matching/OrderBook.cpp

#include "hyperion/matching/OrderBook.hpp"
#include <algorithm>
#include <stdexcept>

namespace hyperion::matching {

void OrderBook::add_order(Order* order) {
    // 1. Safety Check: Ensure the ID doesn't exceed our pre-allocated map
    if (order->order_id >= order_map_.size()) {
        // In a real HFT system, it might resize here or drop the order, 
        // but throwing/logging is ok for current scope.
        throw std::out_of_range("Order ID exceeds order_map_ capacity.");
    }

    // 2. $O(1)$ Pointer Registration
    order_map_[order->order_id] = order;

    // 3. $O(1)$ Array Index Calculation
    size_t idx = price_to_index(order->price);

    // 4. Route, Append, and Update Top of Book
    if (order->side == Side::BUY) {
        bids_[idx].append_order(order);
        
        // The Best Bid is the HIGHEST buying price
        best_bid_price_ = std::max(best_bid_price_, order->price);
    } else {
        asks_[idx].append_order(order);
        
        // The Best Ask is the LOWEST selling price
        best_ask_price_ = std::min(best_ask_price_, order->price);
    }
}

void OrderBook::cancel_order(uint64_t order_id) {
    // 1. Safety Check: Is the ID within our array bounds?
    if (order_id >= order_map_.size()) return;

    // 2. Lookup
    Order* order = order_map_[order_id];
    if (order == nullptr) return; // Order doesn't exist or already canceled

    // 3. Locate Array Slot
    size_t idx = price_to_index(order->price);

    // 4. Execute Pointer Surgery
    if (order->side == Side::BUY) {
        bids_[idx].remove_order(order);

        // 5. Clear the Map
        order_map_[order_id] = nullptr;

        if (order->price == best_bid_price_) update_best_bid();
    } else {
        asks_[idx].remove_order(order);

        // 5. Clear the Map
        order_map_[order_id] = nullptr;
        if (order->price == best_ask_price_) update_best_ask();
    }
}

oid OrderBook::update_best_bid() {
    // Cast to signed integer so can safely drop below 0
    int idx = static_cast<int>(price_to_index(best_bid_price_));
    
    while (idx >= 0 && bids_[idx].is_empty()) {
        --idx;
    }

    // If we fell off the bottom of the array, the book is empty on the bid side
    if (idx < 0) {
        best_bid_price_ = 0;
    } else {
        best_bid_price_ = bids_[idx].price;
    }
}

void OrderBook::update_best_ask() {
    int idx = static_cast<int>(price_to_index(best_ask_price_));
    int max_idx = static_cast<int>(asks_.size() - 1);
    
    while (idx <= max_idx && asks_[idx].is_empty()) {
        ++idx;
    }

    // If we fell off the top of the array, the book is empty on the ask side
    if (idx > max_idx) {
        best_ask_price_ = UINT64_MAX;
    } else {
        best_ask_price_ = asks_[idx].price;    
    }
}

} // namespace hyperion::matching
