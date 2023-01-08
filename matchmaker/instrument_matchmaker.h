#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "order_outcome_types.h"
#include "simple_order_book.h"
#include "trade_event.h"
#include "trade_order.h"

namespace matchmaker {


class InstrumentMatchmaker {
    private:
        const matchmaker::InstrumentType instrument_type_;
        std::unordered_map<uint64_t, matchmaker::SimpleOrderBook> symbol_orderbooks_;
        std::unordered_map<uint64_t, matchmaker::InstrumentSymbol> instrument_symbols_;
    public:
        InstrumentMatchmaker(
            matchmaker::InstrumentType instrument_type
        );
        bool AddSymbol(matchmaker::InstrumentSymbol& instrument_symbol);
        uint16_t GetNumOrderBooks();
        matchmaker::InstrumentType GetInstrumentType();
        OrderOutcomeType ConsumeOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events);
};

}
