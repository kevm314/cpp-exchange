#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <thread>

#include <nlohmann/json.hpp>
#include "spdlog/spdlog.h"

#include "matchmaker/instrument_matchmaker.h"
#include "matchmaker/instrument_symbol.h"
#include "matchmaker/matchmaker_engine.h"
#include "matchmaker/trade_event.h"
#include "matchmaker/trade_order.h"


// TODO: add below into own header/cpp file for running exchange as queue based
/**
 * @brief Run an exchange matchmaker thread per instrument type.
 * Each exchange matchmaker thread manages all orderbooks (1 per symbol) for the instrument type specified.
 * 
 */
void RunInstrumentMatchmakerQueueBased(matchmaker::InstrumentMatchmaker& instrument_matchmaker) {
    spdlog::info("-- Playback mode running " + std::to_string(instrument_matchmaker.GetNumOrderBooks()) + " orderbook(s) for instrument type: " + std::to_string(instrument_matchmaker.GetInstrumentType()));

    // OrderOutcomeType outcome;
    while(true) {
        // TODO: have async method of passing TradeOrder objects into the thread (disruptor pattern OR simple ring buffer => need to plan)
        // std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events = std::make_shared<std::vector<matchmaker::TradeEvent>>();
        // outcome = instrument_matchmaker.ConsumeOrder(trade_order, trade_events);
        // spdlog::info("Trade order request outcome code: " + std::to_string(outcome));
        std::this_thread::sleep_for (std::chrono::milliseconds(10));
    }

    return;
}

// void RunProducerQueueBased() {

// }

void RunExchangeQueueBased(std::unordered_map<matchmaker::InstrumentType, matchmaker::InstrumentMatchmaker>& instrument_type_matchmakers) {
    //TODO: move below to running as queue based
    // TODO: set up csv playback loading to mimic streaming TradeOrder objects into the ring buffer (managed by appropriate thread)
    // set up threads 1 per instrument type to manage matchmaking across all corresponding orderbooks
    if (instrument_type_matchmakers.contains(matchmaker::InstrumentType::CURRENCY_PAIR)) {
        std::thread currencies(RunInstrumentMatchmakerQueueBased, std::ref(instrument_type_matchmakers[matchmaker::InstrumentType::CURRENCY_PAIR]));
        currencies.join();
    }
    if (instrument_type_matchmakers.contains(matchmaker::InstrumentType::FUTURES_CONTRACT)) {
        std::thread futures(RunInstrumentMatchmakerQueueBased, std::ref(instrument_type_matchmakers[matchmaker::InstrumentType::FUTURES_CONTRACT]));
        futures.join();
    }
    if (instrument_type_matchmakers.contains(matchmaker::InstrumentType::OPTION)){
        std::thread options(RunInstrumentMatchmakerQueueBased, std::ref(instrument_type_matchmakers[matchmaker::InstrumentType::OPTION]));
        options.join();
    }
    if (instrument_type_matchmakers.contains(matchmaker::InstrumentType::SHARE)) {
        std::thread shares(RunInstrumentMatchmakerQueueBased, std::ref(instrument_type_matchmakers[matchmaker::InstrumentType::SHARE]));
        shares.join();
    }
}

bool IsValidConfigKeys(nlohmann::json& data) {
    if (data.contains("symbols") && data.contains("instrument_types")) {
        return true;
    }
    spdlog::critical("-- Config file does not contain valid keys");
    return false;
}

/**
 * @brief 
 * 
 * @return int 
 */
int main(int argc, char* argv[]) {
    spdlog::info(R"(    
                                        __                     
         _______  ___  _________ ______/ /  ___ ____  ___ ____ 
        / __/ _ \/ _ \/___/ -_) \ / __/ _ \/ _ `/ _ \/ _ `/ -_)
        \__/ .__/ .__/    \__/_\_\\__/_//_/\_,_/_//_/\_, /\__/ 
          /_/  /_/                                  /___/      
    )");
    spdlog::info("-- Exchange server started");
    if (argv[1] == nullptr) {
        spdlog::critical("No config file path provided - exiting ...");
        return 0;
    }
    nlohmann::json data;
    try {
        std::string filepath(argv[1]);
        spdlog::info("Loading filepath: "+ filepath);
        std::ifstream f(filepath);
        data = nlohmann::json::parse(f);
    } catch(std::exception& e) {
        spdlog::critical("-- Exception caught in loading config file ... exiting:" + std::string(e.what()));
        return 1;
    }
    if (!IsValidConfigKeys(data))
        return 1;
    
    std::unordered_map<matchmaker::InstrumentType, matchmaker::InstrumentMatchmaker> instrument_type_matchmakers;
    std::unordered_map<uint32_t, matchmaker::InstrumentSymbol> symbols_map;

    matchmaker::MatchmakerEngine(
        &instrument_type_matchmakers,
        &symbols_map,
        &data
    );
    RunExchangeQueueBased(instrument_type_matchmakers);

    return 0;
}
