#include <chrono>
#include <iostream>

#include "spdlog/spdlog.h"

#include "matchmaker/instrument_matchmaker.h"
#include "matchmaker/instrument_symbol.h"
#include "matchmaker/trade_event.h"
#include "matchmaker/trade_order.h"

/**
 * @brief Playback function for running an exchange matchmaker thread per instrument type.
 * Each exchange matchmaker thread manages all orderbooks (1 per symbol) for the instrument type specified.
 * 
 */
void RunInstrumentMatchmakerPlayback(matchmaker::InstrumentMatchmaker& instrument_matchmaker) {
    spdlog::info("-- Playback mode running " + std::to_string(instrument_matchmaker.GetNumOrderBooks()) + " orderbook(s) for instrument type: " + std::to_string(instrument_matchmaker.GetInstrumentType()));
    
    std::array<uint8_t, 36> id;
    
    std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events = std::make_shared<std::vector<matchmaker::TradeEvent>>();
    
    // TODO: update below while loop with correct csv playback of orders
    bool is_bid = true;
    OrderOutcomeType outcome;
    while(true) {
        std::fill(id.begin(), id.end(), 1);
        matchmaker::TradeOrder simulated_bid = matchmaker::TradeOrder(
            id,
            id,
            1,
            TradeOrderType::GTC,
            TradeQuotationType::BID,
            OrderRequestType::PLACE_ORDER,
            OrderOutcomeType::NOT_PROCESSED,
            100,
            100,
            1    
        );
        std::fill(id.begin(), id.end(), 2);
        matchmaker::TradeOrder simulated_ask = matchmaker::TradeOrder(
            id,
            id,
            1,
            TradeOrderType::GTC,
            TradeQuotationType::ASK,
            OrderRequestType::PLACE_ORDER,
            OrderOutcomeType::NOT_PROCESSED,
            100,
            100,
            1    
        );
        if (is_bid) {
            outcome = instrument_matchmaker.ConsumeOrder(simulated_bid, trade_events);
        } else {
            outcome = instrument_matchmaker.ConsumeOrder(simulated_ask, trade_events);
        }
        is_bid = !is_bid;
        spdlog::info("Trade order request outcome code: " + std::to_string(outcome));
        std::this_thread::sleep_for (std::chrono::seconds(1));
    }

    return;
}

/**
 * @brief Function which spawns the appropriate number of threads per instrument type based off a configuration file.
 * 
 */
void SpawnExchangeInstrumentMatchmakers() {
    spdlog::info("-- Starting exchange matchmakers");

    // TODO: update mocking input of instrument types and symbols from config file (1 instrument type containing 1 symbol for now)
    matchmaker::InstrumentType instrument_type = matchmaker::InstrumentType::SHARE;
    uint32_t instrument_symbol_id = 1;
    int32_t base_currency = 0;
    int32_t quote_currency = 0;
    int32_t base_multiplier_k = 0;
    int32_t quote_multiplier_k = 0;
    int32_t maker_fee = 1;
    matchmaker::InstrumentSymbol instrument_symbol = matchmaker::InstrumentSymbol(
        instrument_symbol_id,
        "test_symbol",
        instrument_type,
        base_currency,
        quote_currency,
        base_multiplier_k,
        quote_multiplier_k,
        maker_fee
    );

    // TODO: store matchmaker objects per instrument type (so easily dispatchable per business logic thread?)
    matchmaker::InstrumentMatchmaker share_matchmaker = matchmaker::InstrumentMatchmaker(instrument_type);
    // TODO: fix how InstrumentSymbol is copied by value when constructing orderbook
    // for loop for symbol adding
    uint16_t num_symbols = 1;
    for (uint16_t index = 0; index < num_symbols; index++) {
        share_matchmaker.AddSymbol(instrument_symbol);
    }
    // run "thread" function
    RunInstrumentMatchmakerPlayback(share_matchmaker);

    return;
}

/**
 * @brief 
 * 
 * @return int 
 */
int main() {
    spdlog::info(R"(    
                                        __                     
         _______  ___  _________ ______/ /  ___ ____  ___ ____ 
        / __/ _ \/ _ \/___/ -_) \ / __/ _ \/ _ `/ _ \/ _ `/ -_)
        \__/ .__/ .__/    \__/_\_\\__/_//_/\_,_/_//_/\_, /\__/ 
          /_/  /_/                                  /___/      
    )");
    spdlog::info("-- Exchange server started");
    SpawnExchangeInstrumentMatchmakers();
    return 0;
}
