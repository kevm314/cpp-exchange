#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "instrument_matchmaker.h"
#include "order_outcome_types.h"
#include "simple_order_book.h"
#include "trade_event.h"
#include "trade_order.h"

namespace matchmaker {

class MatchmakerEngine {
    private:
        std::unordered_map<matchmaker::InstrumentType, matchmaker::InstrumentMatchmaker>* instrument_type_matchmakers_;
        std::unordered_map<uint32_t, matchmaker::InstrumentSymbol>* symbols_map_;
    public:
        MatchmakerEngine(
            std::unordered_map<matchmaker::InstrumentType, matchmaker::InstrumentMatchmaker>* instrument_type_matchmakers,
            std::unordered_map<uint32_t, matchmaker::InstrumentSymbol>* symbols_map,
            nlohmann::json* data
        );
        void ConfigureInstrumentMatchmakersPerType(
            nlohmann::json* data
        );
        void AddSymbolsForInstrumentType(
            nlohmann::json* data,
            nlohmann::json::iterator& instrument_type_it,
            matchmaker::InstrumentMatchmaker& instrument_matchmaker
        );
};

}
