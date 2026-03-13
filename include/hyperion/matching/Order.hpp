#pragma once

#include <cstdint>

namespace hyperion::matching {

    enum class Side { BUY, SELL };


    struct Order {
        uint64_t order_id;
        uint64_t price;
        uint32_t quantity;
        Side side;

        Order() = default;

        Order *prev{nullptr};
        Order *next{nullptr};
    };

}
