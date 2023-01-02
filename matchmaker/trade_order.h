#pragma once

#include <array>

#include "order_outcome_types.h"
#include "order_request_types.h"
#include "trade_order_types.h"
#include "trade_quotation_types.h"

namespace matchmaker {

/**
 * @brief Class for trade order placing.
 * 
 */
class TradeOrder {
    private:
        const std::array<uint8_t, 32> task_id_;
        const std::array<uint8_t, 32> user_id_; 
        const uint32_t instrument_symbol_id_;
        const TradeOrderType order_type_; 
        const TradeQuotationType quotation_type_; 
        const OrderRequestType request_type_; 
        OrderOutcomeType order_outcome_; 
        uint64_t price_; 
        uint64_t size_; 
        const uint32_t timestamp_;
    public:
        TradeOrder(
            std::array<uint8_t, 32> task_id,
            std::array<uint8_t, 32> user_id,
            uint32_t instrument_symbol_id, 
            TradeOrderType order_type, 
            TradeQuotationType quotation_type, 
            OrderRequestType request_type, 
            OrderOutcomeType order_outcome, 
            uint64_t price, 
            uint64_t size, 
            uint32_t timestamp
        );
        std::array<uint8_t, 32> GetTaskId();
        std::array<uint8_t, 32> GetUserId();
        uint32_t GetInstrumentSymbolId();
        TradeOrderType GetOrderType();
        TradeQuotationType GetQuotationType();
        OrderRequestType GetRequestType();
        OrderOutcomeType GetOrderOutcome();
        uint64_t GetPrice();
        uint64_t GetSize();
        uint32_t GetTimestamp();
};

}
