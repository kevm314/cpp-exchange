#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "order_outcome_types.h"
#include "trade_order.h"

namespace matchmaker {

/**
 * @brief Class for a simple data structure which stores a collection of order objects at the same price point.
 * Stored objects are filled in a FIFO approach.
 * 
 */
class SimplePriceBucket {
    private:
        uint64_t price_; 
        uint64_t total_volume_; 
        matchmaker::TradeOrder* order_list_head_;     
    public:
        SimplePriceBucket(uint64_t price);
        uint64_t GetPrice();
        uint64_t GetTotalVolume();
        matchmaker::TradeOrder* GetFirstOrder();
        bool InsertOrder(matchmaker::TradeOrder& trade_order);
};


}