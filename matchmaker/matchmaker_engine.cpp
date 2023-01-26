#include <stdint.h>

#include <array>
#include <exception>
#include <iostream>
#include <memory>
#include <vector>

#include <nlohmann/json.hpp>
#include "spdlog/spdlog.h"

#include "base_order_book.h"
#include "instrument_matchmaker.h"
#include "instrument_symbol.h"
#include "matchmaker_engine.h"
#include "simple_order_book.h"
#include "simple_order_book_factory.h"
#include "trade_event.h"

namespace matchmaker {

MatchmakerEngine::MatchmakerEngine(
    std::unordered_map<matchmaker::InstrumentType, matchmaker::InstrumentMatchmaker>* instrument_type_matchmakers,
    std::unordered_map<uint32_t, matchmaker::InstrumentSymbol>* symbols_map,
    nlohmann::json* data
):
    instrument_type_matchmakers_(instrument_type_matchmakers),
    symbols_map_(symbols_map)
{
    ConfigureInstrumentMatchmakersPerType(data);
};

void MatchmakerEngine::ConfigureInstrumentMatchmakersPerType(
    nlohmann::json* data
) {
    for (nlohmann::json::iterator instrument_type_it = (*data)["instrument_types"].begin(); instrument_type_it != (*data)["instrument_types"].end(); ++instrument_type_it) {
        matchmaker::InstrumentType instrument_type;
        instrument_type = static_cast<matchmaker::InstrumentType>(std::stoi(instrument_type_it.key(), nullptr, 10));
        // confirm valid enum value such that supported instrument type is configured.
        if (instrument_type < matchmaker::InstrumentType::FIRST_INSTRUMENT || instrument_type > matchmaker::InstrumentType::LAST_INSTRUMENT) {
            spdlog::critical("Unsupported instrument type: " + instrument_type_it.key());
            continue;
        }
        spdlog::info("Processing instrument type: " + instrument_type);
        // create instrument matchmakers class for current instrument type
        std::pair<std::unordered_map<matchmaker::InstrumentType, matchmaker::InstrumentMatchmaker>::iterator, bool> instrument_matchmakers_it = instrument_type_matchmakers_->emplace(
            std::piecewise_construct,
            std::forward_as_tuple(instrument_type),
            std::forward_as_tuple(instrument_type, std::move(std::make_shared<matchmaker::SimpleOrderBookFactory>()))
        );
        if (!instrument_matchmakers_it.second) {
            spdlog::critical("Unable to add matchmakers object for instrument type: " + instrument_type);
            continue;
        }
        AddSymbolsForInstrumentType(data, instrument_type_it, instrument_matchmakers_it.first->second);
    }
    return;
}


void MatchmakerEngine::AddSymbolsForInstrumentType(
    nlohmann::json* data,
    nlohmann::json::iterator& instrument_type_it,
    matchmaker::InstrumentMatchmaker& instrument_matchmaker
) {
    // iterate through defined instrument symbols
    for (nlohmann::json::iterator symbols = instrument_type_it.value().begin(); symbols != instrument_type_it.value().end(); ++symbols) {
        std::string current_symbol_id = std::to_string((*symbols).get<int>());
        uint32_t current_symbol_id_int = (*symbols).get<uint32_t>();
        if (!(*data)["symbols"].contains(current_symbol_id)) {
            spdlog::warn("    Symbol id not found (skipping): " + current_symbol_id);
            continue;
        }
        if (!(*data)["symbols"][current_symbol_id].contains("instrument_name") ||\
            !(*data)["symbols"][current_symbol_id].contains("instrument_type") ||\
            !(*data)["symbols"][current_symbol_id].contains("base_currency") ||\
            !(*data)["symbols"][current_symbol_id].contains("quote_currency") ||\
            !(*data)["symbols"][current_symbol_id].contains("base_multiplier_k") ||\
            !(*data)["symbols"][current_symbol_id].contains("quote_multiplier_k") ||\
            !(*data)["symbols"][current_symbol_id].contains("maker_fee")
        ) {
            spdlog::warn("    Symbol id missing field(s) (skipping): " + current_symbol_id);
            continue;
        }
        uint32_t symbol_specified_instrument_type = (*data)["symbols"][current_symbol_id]["instrument_type"].get<uint32_t>();
        if (symbol_specified_instrument_type != instrument_matchmaker.GetInstrumentType()) {
            spdlog::warn("    Symbol specified instrument type is not listed under current instrument type => symbol id: " + current_symbol_id);
            continue;
        }

        spdlog::info("    Adding symbol id: " + current_symbol_id);
        matchmaker::InstrumentType instrument_type = instrument_type;
        std::string instrument_name = (*data)["symbols"][current_symbol_id]["instrument_name"];
        uint32_t instrument_symbol_id = (*symbols).get<int>();
        uint32_t base_currency = (*data)["symbols"][current_symbol_id]["base_currency"].get<uint32_t>();
        uint32_t quote_currency = (*data)["symbols"][current_symbol_id]["quote_currency"].get<uint32_t>();
        uint32_t base_multiplier_k = (*data)["symbols"][current_symbol_id]["base_multiplier_k"].get<uint32_t>();
        uint32_t quote_multiplier_k = (*data)["symbols"][current_symbol_id]["quote_multiplier_k"].get<uint32_t>();
        uint32_t maker_fee = (*data)["symbols"][current_symbol_id]["maker_fee"].get<uint32_t>();
        // create symbol object directly into the symbols map
        std::pair<std::unordered_map<uint32_t, matchmaker::InstrumentSymbol>::iterator,bool> symbols_map_it = symbols_map_->emplace(std::make_pair(
            current_symbol_id_int,
            matchmaker::InstrumentSymbol(
                instrument_symbol_id,
                instrument_name,
                instrument_type,
                base_currency,
                quote_currency,
                base_multiplier_k,
                quote_multiplier_k,
                maker_fee
            )
        ));
        if (!symbols_map_it.second) {
            spdlog::warn("    Unable to add symbol to symbols map => symbol id: " + current_symbol_id);
            continue;
        }
        instrument_matchmaker.AddSymbol(&symbols_map_it.first->second);
    }
 }

}