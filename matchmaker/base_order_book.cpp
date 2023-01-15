#include <stdint.h>

#include <array>
#include <memory>
#include <vector>

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "trade_event.h"

namespace matchmaker {

BaseOrderBook::BaseOrderBook(
    matchmaker::InstrumentSymbol* instrument_symbol
):
    instrument_symbol_(instrument_symbol)
{}
const matchmaker::InstrumentSymbol* BaseOrderBook::GetInstrumentSymbol() {
    return instrument_symbol_;
}
OrderOutcomeType BaseOrderBook::ConsumeOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) {
    return OrderOutcomeType::FAIL;
}
OrderOutcomeType BaseOrderBook::ProcessNewOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) {
    return OrderOutcomeType::FAIL;
} 
OrderOutcomeType BaseOrderBook::CancelOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) {
    return OrderOutcomeType::FAIL;
} 
OrderOutcomeType BaseOrderBook::AlterOrderSize(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) {
    return OrderOutcomeType::FAIL;
} 
OrderOutcomeType BaseOrderBook::AlterOrderPrice(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) {
    return OrderOutcomeType::FAIL;
} 
std::vector<matchmaker::TradeOrder> BaseOrderBook::GetUserOrders(const std::array<uint8_t, 32>& user_id) {
    return {};
}
}
