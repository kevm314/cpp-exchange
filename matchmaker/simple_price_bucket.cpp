#include <stdint.h>

#include <algorithm>
#include <array>
#include <memory>
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
    uint32_t instrument_symbol_id,
    uint64_t price
) {
    quotation_type_ = quotation_type;
    instrument_symbol_id_ = instrument_symbol_id;
    price_ = price;
    total_volume_ = 0;
    order_list_head_ = nullptr;
    order_list_tail_ = nullptr;
}
TradeQuotationType SimplePriceBucket::GetQuotationType() {
    return quotation_type_;
}
uint32_t SimplePriceBucket::GetInstrumentSymbolId() {
    return instrument_symbol_id_;
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
    total_volume_ += (trade_order.GetSize() - trade_order.GetFilled());
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
    // reduce by available volume
    total_volume_ -= (trade_order.GetSize() - trade_order.GetFilled());
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
uint64_t SimplePriceBucket::FulfillOrder(matchmaker::TradeOrder& requested_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>>& trade_events) {
    const TradeQuotationType request_quote_needed = quotation_type_ == TradeQuotationType::ASK ? TradeQuotationType::BID : TradeQuotationType::ASK;
    // check if 0 orders in bucket or other invalid reasons
    if (tracked_orders_.size() == 0 || requested_order.GetQuotationType() != request_quote_needed || requested_order.GetPrice() != price_ || requested_order.GetSize() == 0)
        return 0;
    // iterate through the orders to try and match the volume in FIFO manner
    TradeOrder* candidate_order = order_list_head_;
    TradeOrder* candidate_to_delete = candidate_order;
    uint64_t candidate_fulfilled_volume = 0;
    while (candidate_order != nullptr) {
        // check available volume
        // if sufficient volume perform trade (generate trade event)
        candidate_fulfilled_volume = std::min(candidate_order->GetSize() - candidate_order->GetFilled(), requested_order.GetSize() - requested_order.GetFilled());
        // update filled volumes
        candidate_order->SetFilled(candidate_order->GetFilled() + candidate_fulfilled_volume);
        requested_order.SetFilled(requested_order.GetFilled() + candidate_fulfilled_volume);
        total_volume_ -= candidate_fulfilled_volume;
        if (candidate_fulfilled_volume != 0) {
            std::array<uint8_t, 36> bid_trade_id;
            std::array<uint8_t, 36> ask_trade_id;
            if (request_quote_needed == TradeQuotationType::BID) {
                bid_trade_id = requested_order.GetTradeId();
                ask_trade_id = candidate_order->GetTradeId();
            } else {
                bid_trade_id = candidate_order->GetTradeId();
                ask_trade_id = requested_order.GetTradeId();
            }
            spdlog::info("Trade event for symbol: " + std::to_string(instrument_symbol_id_) + " @ price: " + std::to_string(price_) +  " volume traded: " + std::to_string(candidate_fulfilled_volume));
            trade_events->emplace_back(
                bid_trade_id,
                ask_trade_id,
                instrument_symbol_id_,
                price_,
                candidate_fulfilled_volume
            );
        }
        // move to next candidate order and check if any orders filled
        candidate_to_delete = candidate_order;
        candidate_order = candidate_order->GetNextOrder();
        if (candidate_to_delete->GetFilled() == candidate_to_delete->GetSize()) {
            candidate_to_delete->SetOrderOutcome(OrderOutcomeType::SUCCESS);
            EraseOrder(*candidate_to_delete);
        }
        // check if requested order is completely filled
        if (requested_order.GetFilled() == requested_order.GetSize()) {
            requested_order.SetOrderOutcome(OrderOutcomeType::SUCCESS);
            return requested_order.GetFilled();
        }
    }
    // requested order is only partially matched
    return requested_order.GetFilled();
}
}