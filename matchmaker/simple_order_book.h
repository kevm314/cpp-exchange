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
        std::map<uint64_t, SimplePriceBucket> bid_prices_ = {};
        std::map<uint64_t, SimplePriceBucket> ask_prices_ = {};
        std::unordered_map<TradeQuotationType, std::map<uint64_t, SimplePriceBucket>*> quoted_prices_ = {};
        std::unique_ptr<std::unordered_map<std::string, matchmaker::TradeOrder>> orderbook_orders_ = std::make_unique<std::unordered_map<std::string, matchmaker::TradeOrder>>();
    public:
        SimpleOrderBook(
            matchmaker::InstrumentSymbol* instrument_symbol
        );
        SimpleOrderBook(SimpleOrderBook& simple_order_book) = delete;
        virtual OrderOutcomeType ConsumeOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) override;
        virtual OrderOutcomeType ProcessNewOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) override;
        OrderOutcomeType ProcessGtcOrder(
            matchmaker::TradeOrder& trade_order,
            TradeQuotationType counter_quote,
            PriceBucketsIterator& price_buckets,
            std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events
        );
        OrderOutcomeType ProcessIocOrder(
            matchmaker::TradeOrder& trade_order,
            TradeQuotationType counter_quote,
            PriceBucketsIterator& price_buckets,
            std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events
        );
        virtual OrderOutcomeType CancelOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) override;
        virtual OrderOutcomeType AlterOrderSize(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) override;
        virtual OrderOutcomeType AlterOrderPrice(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) override;
        // virtual std::vector<matchmaker::TradeOrder> GetUserOrders(const std::array<uint8_t, 32>& user_id) override;
        bool DoesTradeExist(std::string trade_id);
        bool IsValidSymbolAndSize(matchmaker::TradeOrder& trade_order);
        PriceBucketsIterator GetCandidatePriceBuckets(uint64_t price, TradeQuotationType counter_quote);
        /**
         * @brief Create a Price Bucket object if price key does not exist
         * 
         * @param price 
         * @param counter_quote 
         * @return true if created
         * @return false if not created
         */
        bool CreatePriceBucket(uint64_t price, TradeQuotationType counter_quote);
        uint64_t GetNumberBuckets(TradeQuotationType counter_quote);
        OrderOutcomeType IterativelyFulfillOrder(
            matchmaker::TradeOrder& trade_order,
            TradeQuotationType counter_quote,
            PriceBucketsIterator& price_buckets,
            std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events
        );
        uint64_t GetNumOrdersAtPrice(uint64_t price, TradeQuotationType quote_type);
        uint64_t GetVolumeAtPrice(uint64_t price, TradeQuotationType quote_type);
        bool RemoveOrderFromBucketAndOrderBook(matchmaker::TradeOrder* existing_order);
        matchmaker::TradeOrder* FindMatchingTradeOrder(matchmaker::TradeOrder trade_order, OrderOutcomeType& outcome);
};

}
