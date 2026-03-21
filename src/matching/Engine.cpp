// src/matching/Engine.cpp

#include "hyperion/matching/Engine.hpp"

namespace hyperion::matching {

void Engine::process_order(Order* order) {
    if (order->side == Side::BUY) {
        
        // --- PHASE 1: AGGRESSIVE MATCHING ---
        // Loop while I still need shares AND there is a seller AND their price is low enough for me
        while (order->quantity > 0 && 
               book_.best_ask_price_ <= order->price && 
               book_.best_ask_price_ != UINT64_MAX) {
            
            // 1. Find the top of the line
            size_t ask_idx = book_.price_to_index(book_.best_ask_price_);
            PriceLevel& best_ask_level = book_.asks_[ask_idx];
            Order* resting_ask = best_ask_level.head;

            if (resting_ask == nullptr) {
                // Should never happen with perfect state tracking, but safety first
                book_.update_best_ask();
                continue;
            }

            // 2. Calculate Trade Quantity
            // TODO: The trade size is the std::min() between the incoming order's quantity and the resting order's quantity
            uint32_t trade_qty = std::min(order->quantity, resting_ask->quantity);

            // 3. Execute the Math
            // TODO: Subtract trade_qty from order->quantity
            // TODO: Subtract trade_qty from resting_ask->quantity
            // TODO: Subtract trade_qty from best_ask_level.total_volume
            order->quantity -= trade_qty;
            resting_ask->quantity -= trade_qty;
            best_ask_level.total_volume -= trade_qty;

            // (In the future, we will create an ExecutionReport here and send it to the lock-free Outbound Queue!)

            // 4. Cleanup Dead Orders
            if (resting_ask->quantity == 0) {
                // TODO: The seller is completely filled. Use book_.cancel_order(...) to remove them.
                // Note: cancel_order will automatically call update_best_ask() if the level empties out!
                book_.cancel_order(resting_ask->order_id);
            }
        }

        // --- PHASE 2: RESTING ---
        // If the buyer ate everything they could but still wants more shares, put them in the book.
        if (order->quantity > 0) {
            // TODO: Call book_.add_order(order)
            book_.add_order(order);
        }

    } else { // order->side == Side::SELL
        
        // --- PHASE 1: AGGRESSIVE MATCHING (SELLER) ---
        while (order->quantity > 0 && 
                book_.best_bid_price_ >= order->price &&
                book_.best_bid_price_ != 0) {

            size_t bid_idx = book_.price_to_index(book_.best_bid_price_);
            PriceLevel& best_bid_level = book_.bids_[bid_idx];
            Order* resting_bid = best_bid_level.head;

            if (resting_bid == nullptr) {
                book_.update_best_bid();
                continue;
            }

            uint32_t trade_qty = std::min(order->quantity, resting_bid->quantity);

            order->quantity -= trade_qty;
            resting_bid->quantity -= trade_qty;
            best_bid_level.total_volume -= trade_qty;

            if (resting_bid->quantity == 0) {
                book_.cancel_order(resting_bid->order_id);
            }
        }
        
        // --- PHASE 2: RESTING (SELLER) ---
        // TODO: If order->quantity > 0, call book_.add_order(order)
        if (order->quantity > 0) {
            // TODO: Call book_.add_order(order)
            book_.add_order(order);
        }
    }
}

} // namespace hyperion::matching