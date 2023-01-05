#include <stdint.h>

#include <array>
#include <vector>

#include "spdlog/spdlog.h"

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "simple_order_book.h"
#include "simple_price_bucket.h"
#include "trade_quotation_types.h"

namespace matchmaker {

SimplePriceBucket::SimplePriceBucket(
    TradeQuotationType quotation_type, 
    uint64_t price
) {
    quotation_type_ = quotation_type;
    price_ = price;
    total_volume_ = 0;
    order_list_head_ = nullptr;
    order_list_tail_ = nullptr;
}
TradeQuotationType SimplePriceBucket::GetQuotationType() {
    return quotation_type_;
}
uint64_t SimplePriceBucket::GetPrice() {
    return price_;
}
uint64_t SimplePriceBucket::GetTotalVolume() {
    return total_volume_;
}
matchmaker::TradeOrder* SimplePriceBucket::GetFirstOrder() {
    return order_list_head_;
}
matchmaker::TradeOrder* SimplePriceBucket::GetLastOrder() {
    return order_list_tail_;
}
bool SimplePriceBucket::IsOrderInserted(const matchmaker::TradeOrder& trade_order) {
    return tracked_orders_.contains(trade_order.GetTradeIdAsString());
}
bool SimplePriceBucket::InsertOrder(matchmaker::TradeOrder& trade_order) {
    std::string trade_id = trade_order.GetTradeIdAsString();
    // check if order already added
    if (tracked_orders_.contains(trade_id)) {
        return false;
    }
    // Check if order correct type (quotation, price and size)
    if (trade_order.GetQuotationType() != quotation_type_ || trade_order.GetPrice() != price_ || trade_order.GetSize() == 0) {
        return false;
    }
    // if tail is null add as first entry otherwise append accordingly
    if (order_list_tail_ == nullptr) {
        order_list_head_ = &trade_order;
        order_list_tail_ = &trade_order;
        trade_order.SetNextOrder(nullptr);
        trade_order.SetPrevOrder(nullptr);
    } else {
        order_list_tail_->SetNextOrder(&trade_order);
        trade_order.SetPrevOrder(order_list_tail_);
        order_list_tail_ = &trade_order;
        trade_order.SetNextOrder(nullptr);
    }
    total_volume_ += trade_order.GetSize();
    // add order to tracked map
    tracked_orders_[trade_id] = &trade_order;
    return true;
}
bool SimplePriceBucket::EraseOrder(matchmaker::TradeOrder& trade_order) {
    std::string trade_id = trade_order.GetTradeIdAsString();
    // check if order already added
    if (!tracked_orders_.contains(trade_id)) {
        return false;
    }
    // reduce volume
    total_volume_ -= trade_order.GetSize();
    // TODO: manage abnormal scenario
    if (total_volume_ < 0) {
        spdlog::critical("Simple price bucket has negative volume, adjusting to 0");
        total_volume_ = 0;
    }
    // update trade orders sequence
    if (tracked_orders_.size() == 1) {
        // single element case
        order_list_tail_ = nullptr;
    } else if (order_list_head_ == &trade_order) {
        // head case
        order_list_head_ = trade_order.GetNextOrder();
        trade_order.GetNextOrder()->SetPrevOrder(nullptr);
    } else if (order_list_tail_ == &trade_order) {
        // tail case
        order_list_tail_ = trade_order.GetPrevOrder();
        trade_order.GetPrevOrder()->SetNextOrder(nullptr);
    } else {
        // somewhere in middle of sequence
        trade_order.GetPrevOrder()->SetNextOrder(trade_order.GetNextOrder());
        trade_order.GetNextOrder()->SetPrevOrder(trade_order.GetPrevOrder());
    }
    trade_order.SetNextOrder(nullptr);
    trade_order.SetPrevOrder(nullptr);
    // remove from map
    tracked_orders_.erase(trade_id);
    return true;
}
}