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
uint64_t SimpleOrderBook::GetNumOrdersAtPrice(uint64_t price, TradeQuotationType quote_type) {
    std::map<uint64_t, matchmaker::SimplePriceBucket>::iterator price_bucket = (*quoted_prices_[quote_type]).find(price);
    if (price_bucket == (*quoted_prices_[quote_type]).end())
        return 0;
    return price_bucket->second.GetNumOrders();
}
uint64_t SimpleOrderBook::GetVolumeAtPrice(uint64_t price, TradeQuotationType quote_type) {
    std::map<uint64_t, matchmaker::SimplePriceBucket>::iterator price_bucket = (*quoted_prices_[quote_type]).find(price);
    if (price_bucket == (*quoted_prices_[quote_type]).end())
        return 0;
    return price_bucket->second.GetTotalVolume();
}
OrderOutcomeType SimpleOrderBook::ConsumeOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) {
    // TODO: consume a generic order book task which is funnelled below (need a way of storing a generic TradeTask message (passed in via TaskProducer + ringbuffer))
    switch(trade_order.GetRequestType()) {
        case OrderRequestType::PLACE_ORDER:
            return ProcessNewOrder(trade_order, trade_events);
        case OrderRequestType::CANCEL_ORDER:
            return CancelOrder(trade_order, trade_events);
        case OrderRequestType::ALTER_ORDER_PRICE:
            return AlterOrderPrice(trade_order, trade_events);
        case OrderRequestType::ALTER_ORDER_SIZE:
            return AlterOrderSize(trade_order, trade_events);
        default:
            spdlog::critical("Unsupported order request type submitted for trade id: " + trade_order.GetTradeIdAsString());
            return OrderOutcomeType::INVALID_REQUEST_TYPE;
    }
    return OrderOutcomeType::INVALID_REQUEST_TYPE;
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
    // funnel to correct order type
    switch(trade_order.GetOrderType()) {
        case TradeOrderType::GTC:
            return ProcessGtcOrder(trade_order, counter_quote, buckets_found, trade_events);
        case TradeOrderType::IOC:
            return ProcessIocOrder(trade_order, counter_quote, buckets_found, trade_events);
        default:
            spdlog::critical("Unsupported trade order type submitted for trade id: " + trade_order.GetTradeIdAsString());
            return OrderOutcomeType::FAIL;
    };
    return OrderOutcomeType::FAIL;
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
    std::pair<std::unordered_map<std::string, matchmaker::TradeOrder>::iterator, bool> new_order_pair = (*orderbook_orders_).emplace(std::make_pair(trade_order.GetTradeIdAsString(), trade_order));
    if (!new_order_pair.second)
        return OrderOutcomeType::ORDER_PARTIALLY_FILLED_INSERTION_ERROR;
    // create a new price bucket (only when needing to place new order into requested quote price buckets)
    CreatePriceBucket(trade_order.GetPrice(), trade_order.GetQuotationType());
    std::map<uint64_t, matchmaker::SimplePriceBucket>::iterator price_bucket = (*quoted_prices_[trade_order.GetQuotationType()]).find(trade_order.GetPrice());
    if (price_bucket == (*quoted_prices_[trade_order.GetQuotationType()]).end())
        return OrderOutcomeType::ORDER_PARTIALLY_FILLED_INSERTION_ERROR;
    price_bucket->second.InsertOrder(new_order_pair.first->second);
    return order_outcome;
}
OrderOutcomeType SimpleOrderBook::ProcessIocOrder(
    matchmaker::TradeOrder& trade_order,
    TradeQuotationType counter_quote,
    PriceBucketsIterator& price_buckets,
    std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events
) {
    return IterativelyFulfillOrder(trade_order, counter_quote, price_buckets, trade_events);
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
OrderOutcomeType SimpleOrderBook::CancelOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) {
    OrderOutcomeType outcome;
    TradeOrder* existing_order = FindMatchingTradeOrder(trade_order, outcome);
    if (existing_order == nullptr)
        return outcome;
    if (!RemoveOrderFromBucketAndOrderBook(existing_order))
        return OrderOutcomeType::ORDER_EXISTS_CANCEL_ERROR;
    return OrderOutcomeType::ORDER_CANCEL_SUCCESS;
}
OrderOutcomeType SimpleOrderBook::AlterOrderSize(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) {
    OrderOutcomeType outcome;
    TradeOrder* existing_order = FindMatchingTradeOrder(trade_order, outcome);
    if (existing_order == nullptr)
        return outcome;
    // only support reduction to a non-zero unfulfilled amount (otherwise order cancel operation must be used)
    if (trade_order.GetSize() == 0 || (existing_order->GetFilled() == trade_order.GetSize()))
        return OrderOutcomeType::NOT_PROCESSED;
    // get price bucket from appropriate bid/ask data store
    // reduce order size via the price bucket
    std::map<uint64_t, matchmaker::SimplePriceBucket>::iterator price_bucket = (*quoted_prices_[existing_order->GetQuotationType()]).find(existing_order->GetPrice());
    if (price_bucket == (*quoted_prices_[existing_order->GetQuotationType()]).end())
        return OrderOutcomeType::ORDER_EXISTS_SIZE_CHANGE_ERROR;
    if (price_bucket->second.AlterOrderSize(existing_order->GetTradeIdAsString(), trade_order.GetSize()))
        return OrderOutcomeType::SUCCESS;
    return OrderOutcomeType::FAIL;
}
OrderOutcomeType SimpleOrderBook::AlterOrderPrice(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) {
    OrderOutcomeType outcome;
    TradeOrder* existing_order = FindMatchingTradeOrder(trade_order, outcome);
    if (existing_order == nullptr)
        return outcome;
    if (existing_order->GetPrice() == trade_order.GetPrice())
        return OrderOutcomeType::NOT_PROCESSED;
    matchmaker::TradeOrder copied_order = *existing_order;
    if (!RemoveOrderFromBucketAndOrderBook(existing_order))
        return OrderOutcomeType::ORDER_EXISTS_PRICE_CHANGE_ERROR;
    // process existing trade order as new order after price update
    if (!copied_order.SetNewPrice(trade_order.GetPrice()))
        return OrderOutcomeType::ORDER_EXISTS_PRICE_CHANGE_ERROR;
    return ProcessNewOrder(copied_order, trade_events);
}
bool SimpleOrderBook::RemoveOrderFromBucketAndOrderBook(matchmaker::TradeOrder* existing_order) {
    // get price bucket from appropriate bid/ask data store
    std::map<uint64_t, matchmaker::SimplePriceBucket>::iterator price_bucket_it = (*quoted_prices_[existing_order->GetQuotationType()]).find(existing_order->GetPrice());
    if (price_bucket_it == (*quoted_prices_[existing_order->GetQuotationType()]).end())
        return false;
    // erase order from price bucket (check bucket empty deletion too) and then from main order book map
    price_bucket_it->second.EraseOrder(*existing_order);
    if (price_bucket_it->second.GetNumOrders() == 0)
        (*quoted_prices_[existing_order->GetQuotationType()]).erase(price_bucket_it->second.GetPrice());
    orderbook_orders_->erase(existing_order->GetTradeIdAsString());
    return true;
}
matchmaker::TradeOrder* SimpleOrderBook::FindMatchingTradeOrder(matchmaker::TradeOrder trade_order, OrderOutcomeType& outcome) {
    // check if order id exists and that user ids match, and that price is different
    std::unordered_map<std::string, matchmaker::TradeOrder>::iterator existing_order_it = (*orderbook_orders_).find(trade_order.GetTradeIdAsString());
    if (existing_order_it == (*orderbook_orders_).end()) {
        outcome = OrderOutcomeType::ORDER_ID_NON_EXISTENT;
        return nullptr;
    }
    TradeOrder* existing_order = &(existing_order_it->second);
    if (existing_order->GetUserId() != trade_order.GetUserId()) {
        outcome = OrderOutcomeType::NON_USER_ACCESS_TO_ORDER;
        return nullptr;
    }
    outcome = OrderOutcomeType::SUCCESS;
    return existing_order;
}
}
