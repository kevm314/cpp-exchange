#include <stdint.h>

#include <array>

#include "trade_order.h"

namespace matchmaker {

TradeOrder::TradeOrder(
    std::array<uint8_t, 32> task_id, 
    std::array<uint8_t, 32> user_id,
    uint32_t instrument_symbol_id, 
    TradeOrderType order_type, 
    TradeQuotationType quotation_type, 
    OrderRequestType request_type, 
    OrderOutcomeType order_outcome, 
    uint64_t price, 
    uint64_t size, 
    uint32_t timestamp
):
    task_id_(task_id),
    user_id_(user_id),
    instrument_symbol_id_(instrument_symbol_id), 
    order_type_(order_type), 
    quotation_type_(quotation_type), 
    request_type_(request_type), 
    order_outcome_(order_outcome), 
    price_(price), 
    size_(size), 
    timestamp_(timestamp)
{}
std::array<uint8_t, 32> TradeOrder::GetTaskId() {
    return task_id_;
}
std::array<uint8_t, 32> TradeOrder::GetUserId() {
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
uint32_t TradeOrder::GetTimestamp() {
    return timestamp_;
}


}
