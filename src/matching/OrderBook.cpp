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

} // namespace hyperion::matching