#include <stdint.h>

#include <array>
#include <vector>

#include "spdlog/spdlog.h"

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "simple_order_book.h"
#include "trade_event.h"

namespace matchmaker {


PriceBucketsIterator::PriceBucketsIterator(
    std::map<uint64_t, matchmaker::SimplePriceBucket>* price_buckets,
    TradeQuotationType counter_quote,
    uint64_t price
):
    price_buckets_(price_buckets),
    counter_quote_(counter_quote),
    price_(price)
{
    if (counter_quote == TradeQuotationType::BID) {
        rbegin_ = (*price_buckets).rbegin();
    } else {
        begin_ = (*price_buckets).begin();
    }
}
bool PriceBucketsIterator::IncrementToNextBestPrice() {
    // cannot increment already invalid iterator
    if (!IsIteratorValid())
        return false;
    if (counter_quote_ == TradeQuotationType::BID) {
        rbegin_++;
        return IsIteratorValid();
    } else {
        begin_++;
        return IsIteratorValid();
    }
}
bool PriceBucketsIterator::IsIteratorValid() {
    if (counter_quote_ == TradeQuotationType::BID) {
        // return false for no available buckets or if price is below requested ask
        if (rbegin_ == (*price_buckets_).rend() || rbegin_->first < price_)
            return false;
    } else {
        // return false for no available buckets or if price is above requested bid
        if (begin_ == (*price_buckets_).end() || begin_->first > price_)
            return false;
    }
    return true;
}
uint64_t PriceBucketsIterator::GetCurrentPrice() {
    if (counter_quote_ == TradeQuotationType::BID) {
        return rbegin_->first;
    } else {
        return begin_->first;
    }
}
SimplePriceBucket& PriceBucketsIterator::GetCurrentBucket() {
    if (counter_quote_ == TradeQuotationType::BID) {
        return rbegin_->second;
    } else {
        return begin_->second;
    }
}

SimpleOrderBook::SimpleOrderBook(
    matchmaker::InstrumentSymbol instrument_symbol
):
    BaseOrderBook(instrument_symbol)
{}
bool SimpleOrderBook::DoesTradeExist(std::string trade_id) {
    return (*orderbook_orders_).contains(trade_id);
}
bool SimpleOrderBook::IsValidSymbolAndSize(matchmaker::TradeOrder& trade_order) {
    // must equal orderbook symbol id and size must be positive non-zero integer
    return ((trade_order.GetInstrumentSymbolId() == GetInstrumentSymbol().GetInstrumentSymbolId()) && (trade_order.GetSize() > 0));
}
PriceBucketsIterator SimpleOrderBook::GetCandidatePriceBuckets(uint64_t price, TradeQuotationType counter_quote) {
    if (counter_quote == TradeQuotationType::BID) {
        // (looking for buyers) if counter quote is bid use iterator to start at highest price and decrement to ask price (want bid prices at or above ask price)
        return PriceBucketsIterator(&bid_prices_, counter_quote, price);
    } else {
        // (looking for sellers) if counter quote is ask use iterator to start at lowest price and increment to bid price (want ask prices at or below bid price)
        return PriceBucketsIterator(&ask_prices_, counter_quote, price);
    }
}
bool SimpleOrderBook::CreatePriceBucket(uint64_t price, TradeQuotationType counter_quote) {
    if (counter_quote == TradeQuotationType::BID) {
        if (bid_prices_.contains(price))
            return false;
        bid_prices_.emplace(std::make_pair(price, matchmaker::SimplePriceBucket(counter_quote, GetInstrumentSymbol().GetInstrumentSymbolId(), price)));
    } else {
        if (ask_prices_.contains(price))
            return false;
        ask_prices_.emplace(std::make_pair(price, matchmaker::SimplePriceBucket(counter_quote, GetInstrumentSymbol().GetInstrumentSymbolId(), price)));
    }
    return true;
}
uint64_t SimpleOrderBook::GetNumberBuckets(TradeQuotationType counter_quote) {
    if (counter_quote == TradeQuotationType::BID) {
        return bid_prices_.size();
    } else {
        return ask_prices_.size();
    }
    return 0;
}
OrderOutcomeType SimpleOrderBook::ConsumeOrder(matchmaker::TradeOrder& trade_order) {
    // TODO: consume a generic order book task which is funnelled below (need a way of storing a generic TradeTask message (passed in via TaskProducer + ringbuffer))
    // TODO: update here
    switch(trade_order.GetRequestType()) {
        case OrderRequestType::PLACE_ORDER:
            break;
        default:
            spdlog::critical("Unsupported order request type submitted for trade id: " + trade_order.GetTradeIdAsString());
            return OrderOutcomeType::FAIL;
    }
    return OrderOutcomeType::SUCCESS;
}
OrderOutcomeType SimpleOrderBook::ProcessNewOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) {
    // check instrument symbol, volume
    if (!IsValidSymbolAndSize(trade_order))
        return OrderOutcomeType::INVALID_SYMBOL_OR_SIZE;
    if (orderbook_orders_->contains(trade_order.GetTradeIdAsString()))
        return OrderOutcomeType::ORDER_ID_ALREADY_PLACED_ERROR;
    TradeQuotationType counter_quote = trade_order.GetQuotationType() == TradeQuotationType::ASK ? TradeQuotationType::BID : TradeQuotationType::ASK;
    // check if price bucket exists
    PriceBucketsIterator buckets_found = GetCandidatePriceBuckets(trade_order.GetPrice(), counter_quote);
    

    // TODO: move to sub method calls
    // if (!buckets_found.IsIteratorValid() && (trade_order.GetOrderType() == TradeOrderType::FOK || trade_order.GetOrderType() == TradeOrderType::IOC))
    //     return OrderOutcomeType::INSUFFICIENT_LIQUIDITY;
    // TODO: finish rest of this method

    // funnel to correct order type
    switch(trade_order.GetOrderType()) {
        case TradeOrderType::GTC:
            return ProcessGtcOrder(trade_order, counter_quote, buckets_found, trade_events);
        default:
            spdlog::critical("Unsupported trade order type submitted for trade id: " + trade_order.GetTradeIdAsString());
            return OrderOutcomeType::FAIL;
    };
    return OrderOutcomeType::SUCCESS;
}
OrderOutcomeType SimpleOrderBook::ProcessGtcOrder(
    matchmaker::TradeOrder& trade_order,
    TradeQuotationType counter_quote,
    PriceBucketsIterator& price_buckets,
    std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events
) {
    OrderOutcomeType order_outcome = IterativelyFulfillOrder(trade_order, counter_quote, price_buckets, trade_events);
    // check if requested order has been fulfilled
    if (order_outcome == OrderOutcomeType::ORDER_COMPLETELY_FILLED)
        return order_outcome;
    // add new order into map for tracking by the orderbook
    (*orderbook_orders_).emplace(std::make_pair(trade_order.GetTradeIdAsString(), trade_order));
    // create a new price bucket (only when needing to place new order into requested quote price buckets)
    CreatePriceBucket(trade_order.GetPrice(), trade_order.GetQuotationType());
    std::map<uint64_t, matchmaker::SimplePriceBucket>::iterator price_bucket = (*quoted_prices_[trade_order.GetQuotationType()]).find(trade_order.GetPrice());
    if (price_bucket == (*quoted_prices_[trade_order.GetQuotationType()]).end())
        return OrderOutcomeType::ORDER_PARTIALLY_FILLED_INSERTION_ERROR;
    price_bucket->second.InsertOrder(trade_order);
    return order_outcome;
}
OrderOutcomeType SimpleOrderBook::IterativelyFulfillOrder(
    matchmaker::TradeOrder& trade_order,
    TradeQuotationType counter_quote,
    PriceBucketsIterator& price_buckets,
    std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events
) {
    // iterate through the price buckets
    while (price_buckets.IsIteratorValid()) {
        // call fulfill order for current bucket and accumulate trade events
        // accumulate list of trade orders to delete from order book map
        std::vector<std::string> trades_to_remove;
        price_buckets.GetCurrentBucket().FulfillOrder(trade_order, trade_events, trades_to_remove);
        for (auto trade_id : trades_to_remove)
            orderbook_orders_->erase(trade_id);
        // increment price bucket and check if recently processed bucket can be removed
        SimplePriceBucket* bucket_deletion_check = &price_buckets.GetCurrentBucket();
        bool is_iterator_valid = price_buckets.IncrementToNextBestPrice();
        if (bucket_deletion_check->GetNumOrders() == 0)
            (*quoted_prices_[counter_quote]).erase(bucket_deletion_check->GetPrice());
        // check if requested order has been filled to exit early
        if (trade_order.GetFilled() == trade_order.GetSize())
            return OrderOutcomeType::ORDER_COMPLETELY_FILLED; 
        if (!is_iterator_valid)
            break;
    }
    if (trade_order.GetFilled() == 0)
        return OrderOutcomeType::ORDER_NOT_FILLED;
    return OrderOutcomeType::ORDER_PARTIALLY_FILLED;
}

}
