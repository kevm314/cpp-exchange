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

class PriceBucketsIterator {
    private:
        std::map<uint64_t, matchmaker::SimplePriceBucket>* price_buckets_;
        TradeQuotationType counter_quote_;
        uint64_t price_;
        std::map<uint64_t, matchmaker::SimplePriceBucket>::iterator begin_;
        std::reverse_iterator<std::map<uint64_t, matchmaker::SimplePriceBucket>::iterator> rbegin_;
    public:
        PriceBucketsIterator(
            std::map<uint64_t, matchmaker::SimplePriceBucket>* price_buckets,
            TradeQuotationType counter_quote,
            uint64_t price
        );
        bool IncrementToNextBestPrice();
        bool IsIteratorValid();
        uint64_t GetCurrentPrice();
        SimplePriceBucket& GetCurrentBucket();
};

/**
 * @brief Derived class for order books. Initial unoptimised implementation (hence simple).
 * 
 */
class SimpleOrderBook: public BaseOrderBook {
    private:
        // TODO: update -> Bid/ask prices stored as (price -> orders bucket), and then in a bucket (order id -> order object)
        std::map<uint64_t, SimplePriceBucket> bid_prices_;
        std::map<uint64_t, SimplePriceBucket> ask_prices_;

    public:
        SimpleOrderBook(
            matchmaker::InstrumentSymbol instrument_symbol
        );
        virtual OrderOutcomeType ConsumeOrder(matchmaker::TradeOrder& trade_order) override;
        virtual OrderOutcomeType ProcessNewOrder(matchmaker::TradeOrder& trade_order) override;
        OrderOutcomeType ProcessGtcOrder(matchmaker::TradeOrder& trade_order);
        // virtual OrderOutcomeType CancelOrder(matchmaker::TradeOrder& trade_order) override;
        // virtual OrderOutcomeType ReduceOrderSize(matchmaker::TradeOrder& trade_order) override;
        // virtual OrderOutcomeType AlterOrderPrice(matchmaker::TradeOrder& trade_order) override;
        // virtual std::vector<matchmaker::TradeOrder> GetUserOrders(const std::array<uint8_t, 32>& user_id) override;
        bool IsValidSymbolAndSize(matchmaker::TradeOrder& trade_order);
        PriceBucketsIterator GetCandidatePriceBuckets(uint64_t price, TradeQuotationType counter_quote);
        void CreatePriceBucket(uint64_t price, TradeQuotationType counter_quote);
        uint64_t GetNumberBuckets(TradeQuotationType counter_quote);
        std::map<uint64_t, matchmaker::SimplePriceBucket>::iterator GetEndOfBuckets(TradeQuotationType counter_quote);
};

}
