#include <stdint.h>

#include <array>
#include <memory>
#include <vector>

#include "spdlog/spdlog.h"

#include "base_order_book.h"
#include "instrument_matchmaker.h"
#include "instrument_symbol.h"
#include "order_book_factory.h"
#include "simple_order_book.h"
#include "simple_order_book_factory.h"
#include "trade_event.h"

namespace matchmaker {

InstrumentMatchmaker::InstrumentMatchmaker():
    instrument_type_(matchmaker::InstrumentType::SHARE)
{
    orderbook_factory_ = std::make_shared<matchmaker::SimpleOrderBookFactory>();
}
InstrumentMatchmaker::InstrumentMatchmaker(
    matchmaker::InstrumentType instrument_type,
    std::shared_ptr<matchmaker::OrderBookFactory> orderbook_factory
):
    instrument_type_(instrument_type),
    orderbook_factory_(orderbook_factory)
{}
bool InstrumentMatchmaker::AddSymbol(matchmaker::InstrumentSymbol* instrument_symbol) {
    // check incorrect symbol type
    if (instrument_type_ != instrument_symbol->GetInstrumentType()) {
        spdlog::warn("InstrumentMatchmaker cannot symbol with different instrument type");
        return false;
    }
    // check symbol id not already added to symbols map
    if (instrument_symbols_.contains(instrument_symbol->GetInstrumentSymbolId()))
        return false;
    // add to symbols map and then create corresponding orderbook
    instrument_symbols_.emplace(std::make_pair(instrument_symbol->GetInstrumentSymbolId(), instrument_symbol));
    symbol_orderbooks_.emplace(instrument_symbol->GetInstrumentSymbolId(), std::move(orderbook_factory_->FactoryProduce(instrument_symbol)));
    return true;
}
matchmaker::InstrumentSymbol* InstrumentMatchmaker::GetInstrumentSymbol(uint32_t symbol_id) {
    std::unordered_map<uint64_t, matchmaker::InstrumentSymbol*>::iterator symbols_it = instrument_symbols_.find(symbol_id);
    if (symbols_it == instrument_symbols_.end()) {
        return nullptr;
    }
    return symbols_it->second;
}
uint16_t InstrumentMatchmaker::GetNumOrderBooks() {
    return symbol_orderbooks_.size();
}
matchmaker::InstrumentType InstrumentMatchmaker::GetInstrumentType() {
    return instrument_type_;
}
OrderOutcomeType InstrumentMatchmaker::ConsumeOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) {
    std::unordered_map<uint64_t, std::unique_ptr<matchmaker::BaseOrderBook>>::iterator orderbook_it = symbol_orderbooks_.find(trade_order.GetInstrumentSymbolId());
    // check if symbol exists
    if (orderbook_it == symbol_orderbooks_.end()) {
        spdlog::warn("InstrumentMatchmaker received order for unknown symbol id: " + std::to_string(trade_order.GetInstrumentSymbolId()));
        return OrderOutcomeType::FAIL;
    }
        
    // fetch orderbook and consume order accordingly
    return orderbook_it->second->ConsumeOrder(trade_order, trade_events);
}
}
