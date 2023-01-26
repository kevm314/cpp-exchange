#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "order_book_factory.h"
#include "order_outcome_types.h"
#include "simple_order_book.h"
#include "trade_event.h"
#include "trade_order.h"

namespace matchmaker {


class InstrumentMatchmaker {
    private:
        const matchmaker::InstrumentType instrument_type_;
        std::unordered_map<uint64_t, std::unique_ptr<matchmaker::BaseOrderBook>> symbol_orderbooks_;
        std::unordered_map<uint64_t, matchmaker::InstrumentSymbol*> instrument_symbols_;
        std::shared_ptr<matchmaker::OrderBookFactory> orderbook_factory_;
    public:
        InstrumentMatchmaker();
        InstrumentMatchmaker(
            matchmaker::InstrumentType instrument_type,
            std::shared_ptr<matchmaker::OrderBookFactory> orderbook_factory
        );
        bool AddSymbol(matchmaker::InstrumentSymbol* instrument_symbol);
        uint16_t GetNumOrderBooks();
        matchmaker::InstrumentSymbol* GetInstrumentSymbol(uint32_t symbol_id);
        matchmaker::InstrumentType GetInstrumentType();
        OrderOutcomeType ConsumeOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events);
};

}
