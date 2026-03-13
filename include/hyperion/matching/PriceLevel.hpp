#pragma once

#include "Order.hpp"

namespace hyperion::matching {

struct PriceLevel {
    uint64_t price;
    uint64_t total_volume{0};

    Order* head{nullptr};
    Order* tail{nullptr};

    bool is_empty() const {
        return head == nullptr;
    }

    void append_order(Order* order) {
        order->next = nullptr;
        order->prev = tail;

        if (is_empty()) {
            head = order;
            tail = order;
        } else {
            tail->next = order;
            tail = order;
        }

        total_volume += order->quantity;
    }

    void remove_order(Order* order) {
        if (is_empty()) return;

        Order* prev_node = order->prev;
        Order* next_node = order->next;

        if (prev_node != nullptr) {
            prev_node->next = next_node;
        } else {
            head = next_node;
        }

        if (next_node != nullptr) {
            next_node->prev = prev_node;
        } else {
            tail = prev_node;
        }

        total_volume -= order->quantity;

        order->prev = nullptr;
        order->next = nullptr;
    }
};

} // namespace hyperion::matching