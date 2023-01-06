#include <stdint.h>

#include <array>
#include <vector>

#include "spdlog/spdlog.h"

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "simple_order_book.h"

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
void SimpleOrderBook::CreatePriceBucket(uint64_t price, TradeQuotationType counter_quote) {
    if (counter_quote == TradeQuotationType::BID) {
        bid_prices_.emplace(std::make_pair(price, matchmaker::SimplePriceBucket(counter_quote, GetInstrumentSymbol().GetInstrumentSymbolId(), price)));
    } else {
        ask_prices_.emplace(std::make_pair(price, matchmaker::SimplePriceBucket(counter_quote, GetInstrumentSymbol().GetInstrumentSymbolId(), price)));
    }
}
uint64_t SimpleOrderBook::GetNumberBuckets(TradeQuotationType counter_quote) {
    if (counter_quote == TradeQuotationType::BID) {
        return bid_prices_.size();
    } else {
        return ask_prices_.size();
    }
    return 0;
}
std::map<uint64_t, matchmaker::SimplePriceBucket>::iterator SimpleOrderBook::GetEndOfBuckets(TradeQuotationType counter_quote) {
    if (counter_quote == TradeQuotationType::BID) {
        return bid_prices_.end();
    } else {
        return ask_prices_.end();
    }
}
OrderOutcomeType SimpleOrderBook::ConsumeOrder(matchmaker::TradeOrder& trade_order) {
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
OrderOutcomeType SimpleOrderBook::ProcessNewOrder(matchmaker::TradeOrder& trade_order) {
    // check instrument symbol, volume
    if (!IsValidSymbolAndSize(trade_order))
        return OrderOutcomeType::INVALID_SYMBOL_OR_SIZE;
    TradeQuotationType counter_quote = trade_order.GetQuotationType() == TradeQuotationType::ASK ? TradeQuotationType::BID : TradeQuotationType::ASK;
    // check if price bucket exists
    PriceBucketsIterator buckets_found = GetCandidatePriceBuckets(trade_order.GetPrice(), counter_quote);
    if (!buckets_found.IsIteratorValid() && (trade_order.GetOrderType() == TradeOrderType::FOK || trade_order.GetOrderType() == TradeOrderType::IOC))
        return OrderOutcomeType::INSUFFICIENT_LIQUIDITY;
    // TODO: finish rest of this method
    // funnel to correct order type
    switch(trade_order.GetOrderType()) {
        case TradeOrderType::GTC:
            break;
        default:
            spdlog::critical("Unsupported trade order type submitted for trade id: " + trade_order.GetTradeIdAsString());
            return OrderOutcomeType::FAIL;
    };
    return OrderOutcomeType::SUCCESS;
}
OrderOutcomeType SimpleOrderBook::ProcessGtcOrder(matchmaker::TradeOrder& trade_order) {
    
    // check quotation type to send to correct price bucket
    // GetPriceBucket()
    // TODO: create a new price bucket (only when needing to place new order into requested quote price buckets)
    // CreatePriceBucket(trade_order.GetPrice(), counter_quote);
    
    return OrderOutcomeType::SUCCESS;
}
}