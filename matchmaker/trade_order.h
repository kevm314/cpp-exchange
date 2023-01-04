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
        std::array<uint8_t, 32> trade_id_;
        std::array<uint8_t, 32> user_id_; 
        uint32_t instrument_symbol_id_;
        TradeOrderType order_type_; 
        TradeQuotationType quotation_type_; 
        OrderRequestType request_type_; 
        OrderOutcomeType order_outcome_; 
        uint64_t price_; 
        uint64_t size_; 
        uint32_t timestamp_;
        TradeOrder* prev_order_;
        TradeOrder* next_order_;
    public:
        TradeOrder(
            std::array<uint8_t, 32> trade_id,
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
        TradeOrder(TradeOrder& trade_order);
        TradeOrder(TradeOrder&& trade_order);
        TradeOrder& operator=(TradeOrder&& trade_order);
        std::array<uint8_t, 32> GetTradeId();
        std::array<uint8_t, 32> GetUserId();
        uint32_t GetInstrumentSymbolId();
        TradeOrderType GetOrderType();
        TradeQuotationType GetQuotationType();
        OrderRequestType GetRequestType();
        OrderOutcomeType GetOrderOutcome();
        uint64_t GetPrice();
        uint64_t GetSize();
        uint32_t GetTimestamp();
        TradeOrder* GetPrevOrder();
        void SetPrevOrder(TradeOrder* prev_order);
        TradeOrder* GetNextOrder();
        void SetNextOrder(TradeOrder* next_order);
};

}
