#include <stdint.h>

#include <array>
#include <string>

#include "trade_order.h"

namespace matchmaker {

TradeOrder::TradeOrder(
    std::array<uint8_t, 36> trade_id, 
    std::array<uint8_t, 36> user_id,
    uint32_t instrument_symbol_id, 
    TradeOrderType order_type, 
    TradeQuotationType quotation_type, 
    OrderRequestType request_type, 
    OrderOutcomeType order_outcome, 
    uint64_t price, 
    uint64_t size, 
    uint32_t timestamp
):
    trade_id_(trade_id),
    user_id_(user_id),
    instrument_symbol_id_(instrument_symbol_id), 
    order_type_(order_type), 
    quotation_type_(quotation_type), 
    request_type_(request_type), 
    order_outcome_(order_outcome), 
    price_(price), 
    size_(size), 
    timestamp_(timestamp)
{
    prev_order_ = nullptr;
    next_order_ = nullptr;
    filled_ = 0;
}
TradeOrder::TradeOrder(TradeOrder& trade_order):
    trade_id_(trade_order.GetTradeId()),
    user_id_(trade_order.GetUserId()),
    instrument_symbol_id_(trade_order.GetInstrumentSymbolId()), 
    order_type_(trade_order.GetOrderType()), 
    quotation_type_(trade_order.GetQuotationType()), 
    request_type_(trade_order.GetRequestType()), 
    order_outcome_(trade_order.GetOrderOutcome()), 
    price_(trade_order.GetPrice()), 
    size_(trade_order.GetSize()), 
    timestamp_(trade_order.GetTimestamp()),
    prev_order_(trade_order.GetPrevOrder()),
    next_order_(trade_order.GetNextOrder()),
    filled_(trade_order.GetFilled())
{}
TradeOrder::TradeOrder(TradeOrder&& trade_order):
    trade_id_(trade_order.GetTradeId()),
    user_id_(trade_order.GetUserId()),
    instrument_symbol_id_(trade_order.GetInstrumentSymbolId()), 
    order_type_(trade_order.GetOrderType()), 
    quotation_type_(trade_order.GetQuotationType()), 
    request_type_(trade_order.GetRequestType()), 
    order_outcome_(trade_order.GetOrderOutcome()), 
    price_(trade_order.GetPrice()), 
    size_(trade_order.GetSize()), 
    timestamp_(trade_order.GetTimestamp()),
    prev_order_(trade_order.GetPrevOrder()),
    next_order_(trade_order.GetNextOrder())
{
    trade_order.SetPrevOrder(nullptr);
    trade_order.SetNextOrder(nullptr);
    trade_order.SetFilled(0);
}
TradeOrder& TradeOrder::operator=(TradeOrder&& trade_order) {
    trade_id_ = trade_order.GetTradeId();
    user_id_ = trade_order.GetUserId();
    instrument_symbol_id_ = trade_order.GetInstrumentSymbolId(); 
    order_type_ = trade_order.GetOrderType(); 
    quotation_type_ = trade_order.GetQuotationType(); 
    request_type_ = trade_order.GetRequestType(); 
    order_outcome_ = trade_order.GetOrderOutcome(); 
    price_ = trade_order.GetPrice(); 
    size_ = trade_order.GetSize(); 
    timestamp_ = trade_order.GetTimestamp();
    prev_order_ = trade_order.GetPrevOrder();
    next_order_ = trade_order.GetNextOrder();
    trade_order.SetPrevOrder(nullptr);
    trade_order.SetNextOrder(nullptr);
    trade_order.SetFilled(0);
    return *this;
}
std::array<uint8_t, 36> TradeOrder::GetTradeId() {
    return trade_id_;
}
std::string TradeOrder::GetTradeIdAsString() const {
    return std::string(std::begin(trade_id_), std::end(trade_id_));
}
std::array<uint8_t, 36> TradeOrder::GetUserId() {
    return user_id_;
}
uint32_t TradeOrder::GetInstrumentSymbolId() {
    return instrument_symbol_id_;
}
TradeOrderType TradeOrder::GetOrderType() {
    return order_type_;
}
TradeQuotationType TradeOrder::GetQuotationType() {
    return quotation_type_;
}
OrderRequestType TradeOrder::GetRequestType() {
    return request_type_;
}
OrderOutcomeType TradeOrder::GetOrderOutcome() {
    return order_outcome_;
}
uint64_t TradeOrder::GetPrice() {
    return price_;
}
uint64_t TradeOrder::GetSize() {
    return size_;
}
uint64_t TradeOrder::GetFilled() {
    return filled_;
}
bool TradeOrder::SetFilled(uint64_t fill_amount) {
    if (fill_amount > size_ || fill_amount < 0)
        return false;
    filled_ = fill_amount;
    return true;
}
uint32_t TradeOrder::GetTimestamp() {
    return timestamp_;
}
TradeOrder* TradeOrder::GetPrevOrder() {
    return prev_order_;
}
void TradeOrder::SetPrevOrder(TradeOrder* prev_order) {
    prev_order_ = prev_order;
}
TradeOrder* TradeOrder::GetNextOrder() {
    return next_order_;
}
void TradeOrder::SetNextOrder(TradeOrder* next_order) {
    next_order_ = next_order;
}

}
