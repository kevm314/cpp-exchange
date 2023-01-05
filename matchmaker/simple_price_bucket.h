#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "order_outcome_types.h"
#include "trade_order.h"
#include "trade_quotation_types.h"

namespace matchmaker {

/**
 * @brief Class for a simple data structure which stores a collection of order objects at the same price point.
 * Stored objects are filled in a FIFO approach.
 * 
 */
class SimplePriceBucket {
    private:
        TradeQuotationType quotation_type_; 
        uint64_t price_; 
        uint64_t total_volume_; 
        matchmaker::TradeOrder* order_list_head_;
        matchmaker::TradeOrder* order_list_tail_;
        std::unordered_map<std::string, matchmaker::TradeOrder*> tracked_orders_;     
    public:
        SimplePriceBucket(TradeQuotationType quotation_type, uint64_t price);
        TradeQuotationType GetQuotationType();
        uint64_t GetPrice();
        uint64_t GetTotalVolume();
        matchmaker::TradeOrder* GetFirstOrder();
        matchmaker::TradeOrder* GetLastOrder();
        bool IsOrderInserted(const matchmaker::TradeOrder& trade_order);
        /**
         * @brief 
         * 
         * @param trade_order the object to be inserted
         * @return true if inserted successfully.
         * @return false if not inserted.
         */
        bool InsertOrder(matchmaker::TradeOrder& trade_order);
        bool EraseOrder(matchmaker::TradeOrder& trade_order);
        uint64_t FulfillOrder(matchmaker::TradeOrder& requested_order);
};


}