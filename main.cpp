#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <thread>

#include <nlohmann/json.hpp>
#include "spdlog/spdlog.h"

#include "comms/scope_locked_queue.h"
#include "events_receiver/playback_producer.h"
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
void RunInstrumentMatchmakerQueueBased(
    matchmaker::InstrumentMatchmaker& instrument_matchmaker, 
    comms::ScopeLockedQueue<matchmaker::TradeOrder>& queue
) {
    spdlog::info("-- Playback mode running " + std::to_string(instrument_matchmaker.GetNumOrderBooks()) + " orderbook(s) for instrument type: " + std::to_string(instrument_matchmaker.GetInstrumentType()));

    OrderOutcomeType outcome;
    matchmaker::TradeOrder trade_order;
    while(true) {
        std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events = std::make_shared<std::vector<matchmaker::TradeEvent>>();
        
        if (queue.Dequeue(trade_order)) {
            outcome = instrument_matchmaker.ConsumeOrder(trade_order, trade_events);
            spdlog::info("Trade order request outcome code: " + std::to_string(outcome));
        }
        std::this_thread::sleep_for (std::chrono::milliseconds(10));
    }

    return;
}

void RunProducerQueueBased(
    events_receiver::PlaybackProducer& playback_producer,
    std::unordered_map<uint32_t, matchmaker::InstrumentSymbol>& symbols_map,
    comms::ScopeLockedQueue<matchmaker::TradeOrder>& currencies_queue,
    comms::ScopeLockedQueue<matchmaker::TradeOrder>& futures_queue,
    comms::ScopeLockedQueue<matchmaker::TradeOrder>& options_queue,
    comms::ScopeLockedQueue<matchmaker::TradeOrder>& shares_queue
) {
    // permanently stream trade order events from the csv file
    while(true) {
        std::this_thread::sleep_for (std::chrono::milliseconds(10));
        matchmaker::TradeOrder trade_order = playback_producer.ReceiveEvent();
        std::unordered_map<uint32_t, matchmaker::InstrumentSymbol>::iterator it_type = symbols_map.find(trade_order.GetInstrumentSymbolId());
        if (it_type == symbols_map.end()) {
            spdlog::warn("PP thread - unable to process stream event as unsupported instrument type (symbol not registered)" + std::to_string(trade_order.GetInstrumentSymbolId()));
            continue;
        }
        spdlog::info("PP thread - streaming trade order event of instrument type: " + std::to_string(it_type->second.GetInstrumentType()));
        switch(it_type->second.GetInstrumentType()) {
            case matchmaker::InstrumentType::CURRENCY_PAIR:
                currencies_queue.Enqueue(std::move(trade_order));
                break;
            case matchmaker::InstrumentType::FUTURES_CONTRACT:
                futures_queue.Enqueue(std::move(trade_order));
                break;
            case matchmaker::InstrumentType::OPTION:
                options_queue.Enqueue(std::move(trade_order));
                break;
            case matchmaker::InstrumentType::SHARE:
                shares_queue.Enqueue(std::move(trade_order));
                break;
            default:
                spdlog::warn("PP thread - unable to process stream event as unsupported instrument type");
                break;
        }
    }
}

void RunExchangeQueueBased(
    std::unordered_map<matchmaker::InstrumentType, matchmaker::InstrumentMatchmaker>& instrument_type_matchmakers,
    std::unordered_map<uint32_t, matchmaker::InstrumentSymbol>& symbols_map,
    std::string& playback_path
) {
    // playback producer
    events_receiver::PlaybackProducer playback_producer = events_receiver::PlaybackProducer(1, playback_path);
    if (!playback_producer.IsInstantiated()) {
        spdlog::critical("Error encountered in instantiating the playback producer");
        return;
    }
    // set up queues for each instrument type matchmaking engine
    comms::ScopeLockedQueue<matchmaker::TradeOrder> currencies_queue = comms::ScopeLockedQueue<matchmaker::TradeOrder>();
    comms::ScopeLockedQueue<matchmaker::TradeOrder> futures_queue = comms::ScopeLockedQueue<matchmaker::TradeOrder>();
    comms::ScopeLockedQueue<matchmaker::TradeOrder> options_queue = comms::ScopeLockedQueue<matchmaker::TradeOrder>();
    comms::ScopeLockedQueue<matchmaker::TradeOrder> shares_queue = comms::ScopeLockedQueue<matchmaker::TradeOrder>();

    // TODO: ensure that symbols_map is thread safe
    // set up 1 thread for producing orders
    std::thread playback_producer_thread(
        RunProducerQueueBased,
        std::ref(playback_producer),
        std::ref(symbols_map),
        std::ref(currencies_queue),
        std::ref(futures_queue),
        std::ref(options_queue),
        std::ref(shares_queue)
    );

    // set up threads 1 per instrument type to manage matchmaking across all corresponding orderbooks
    if (instrument_type_matchmakers.contains(matchmaker::InstrumentType::CURRENCY_PAIR)) {
        std::thread currencies(
            RunInstrumentMatchmakerQueueBased, 
            std::ref(instrument_type_matchmakers[matchmaker::InstrumentType::CURRENCY_PAIR]),
            std::ref(currencies_queue)
        );
        currencies.join();
    }
    if (instrument_type_matchmakers.contains(matchmaker::InstrumentType::FUTURES_CONTRACT)) {
        std::thread futures(
            RunInstrumentMatchmakerQueueBased, 
            std::ref(instrument_type_matchmakers[matchmaker::InstrumentType::FUTURES_CONTRACT]),
            std::ref(futures_queue)
        );
        futures.join();
    }
    if (instrument_type_matchmakers.contains(matchmaker::InstrumentType::OPTION)){
        std::thread options(
            RunInstrumentMatchmakerQueueBased, 
            std::ref(instrument_type_matchmakers[matchmaker::InstrumentType::OPTION]),
            std::ref(options_queue)
        );
        options.join();
    }
    if (instrument_type_matchmakers.contains(matchmaker::InstrumentType::SHARE)) {
        std::thread shares(
            RunInstrumentMatchmakerQueueBased, 
            std::ref(instrument_type_matchmakers[matchmaker::InstrumentType::SHARE]),
            std::ref(shares_queue)
        );
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
    if (argv[2] == nullptr) {
        spdlog::critical("No playback file path provided - exiting ...");
        return 0;
    }
    std::string playback_path(argv[2]);
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
    RunExchangeQueueBased(instrument_type_matchmakers, symbols_map, playback_path);

    return 0;
}
