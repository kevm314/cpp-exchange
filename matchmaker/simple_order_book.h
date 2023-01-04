#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "order_outcome_types.h"
#include "simple_price_bucket.h"
#include "trade_order.h"

namespace matchmaker {

/**
 * @brief Derived class for order books. Initial unoptimised implementation (hence simple).
 * 
 */
class SimpleOrderBook: public BaseOrderBook {
    private:
        // TODO: update -> Bid/ask prices stored as (price -> orders bucket), and then in a bucket (order id -> order object)
        std::map<uint64_t, std::unordered_map<std::string, TradeOrder>> bidPrices_;
        std::map<uint64_t, std::unordered_map<std::string, TradeOrder>> askPrices_;

    public:
        SimpleOrderBook(
            matchmaker::InstrumentSymbol instrument_symbol
        );
        // virtual OrderOutcomeType ConsumeOrder(matchmaker::TradeOrder trade_order) override;
        // virtual OrderOutcomeType ProcessNewOrder(matchmaker::TradeOrder trade_order) override;
        // virtual OrderOutcomeType CancelOrder(matchmaker::TradeOrder trade_order) override;
        // virtual OrderOutcomeType ReduceOrderSize(matchmaker::TradeOrder trade_order) override;
        // virtual OrderOutcomeType AlterOrderPrice(matchmaker::TradeOrder trade_order) override;
        // virtual std::vector<matchmaker::TradeOrder> GetUserOrders(const std::array<uint8_t, 32>& user_id) override;
};

}
