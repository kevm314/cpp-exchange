#include <array>
#include <errno.h>
#include <fstream>
#include <iostream>

#include "spdlog/spdlog.h"

#include "playback_producer.h"
#include "simple_order_book.h"
#include "instrument_symbol.h"
#include "order_outcome_types.h"
#include "order_request_types.h"
#include "trade_order.h"
#include "trade_order_types.h"
#include "trade_quotation_types.h"

namespace events_receiver {

PlaybackProducer::PlaybackProducer(int events_store, std::string configuration): BaseProducer(events_store, configuration) {
    std::ifstream fin;
    fin.open(configuration, std::ios::in);
    if (errno != 0) {
        spdlog::critical("Playback producer - file loading error: ");
        std::cout << strerror(errno) << std::endl;
        is_successfully_instantiated_ = false;
        return;
    }
    is_successfully_instantiated_ = true;
    std::string row;
    while (fin >> row) {
        is_successfully_instantiated_ &= ProcessPlaybackentry(row);
    }
    
}
bool PlaybackProducer::ProcessPlaybackentry(std::string playback_entry) {
    std::stringstream ss(playback_entry);
    std::string single_value;
    
    // trade id
    if (!getline(ss, single_value, ','))
        return false;
    std::array<uint8_t, 36> trade_id;
    for (size_t i = 0; i < trade_id.size(); i++)
        trade_id[i] = static_cast<uint8_t>(single_value[i]);

    // user id
    if (!getline(ss, single_value, ','))
        return false;
    std::array<uint8_t, 36> user_id;
    for (size_t i = 0; i < user_id.size(); i++)
        user_id[i] = static_cast<uint8_t>(single_value[i]);

    // instrument_symbol_id
    if (!getline(ss, single_value, ','))
        return false;
    uint32_t instrument_symbol_id;
    try {
        instrument_symbol_id = std::stoi(single_value, nullptr, 10);
    } catch (std::exception& e) {
        spdlog::critical("PlaybackProducer issue in interpreting instrument_symbol_id: " + std::string(e.what()));
        return false;
    }
    // TODO: add exception handling below to ensure that numerical conversion errors are caught
    // TODO: split each conversion step into its own method?
    // order_type
    if (!getline(ss, single_value, ','))
        return false;
    TradeOrderType order_type = TradeOrderType(std::stoi(single_value, nullptr, 10)); 

    // quotation_type
    if (!getline(ss, single_value, ','))
        return false;
    TradeQuotationType quotation_type = TradeQuotationType(std::stoi(single_value, nullptr, 10)); 

    // request_type
    if (!getline(ss, single_value, ','))
        return false;
    OrderRequestType request_type = OrderRequestType(std::stoi(single_value, nullptr, 10));

    // order_outcome
    if (!getline(ss, single_value, ','))
        return false;
    OrderOutcomeType order_outcome = OrderOutcomeType(std::stoi(single_value, nullptr, 10)); 

    // price
    if (!getline(ss, single_value, ','))
        return false;
    uint64_t price = std::stoi(single_value, nullptr, 10); 

    // size
    if (!getline(ss, single_value, ','))
        return false;
    uint64_t size = std::stoi(single_value, nullptr, 10);

    // timestamp
    if (!getline(ss, single_value, ','))
        return false;
    uint32_t timestamp = std::stoi(single_value, nullptr, 10);
    events_.emplace(events_.end(),
        trade_id,
        user_id,
        instrument_symbol_id, 
        order_type,
        quotation_type,
        request_type,
        order_outcome,
        price,
        size,
        timestamp
    );
    return true;
}
matchmaker::TradeOrder PlaybackProducer::ReceiveEvent() {
    // return events as a stream and wrap to start if end is reached
    if (current_event_ == events_.end())
        current_event_ = events_.begin();
    std::list<matchmaker::TradeOrder>::iterator return_event_it(current_event_);
    current_event_++;
    return *return_event_it;
}
bool PlaybackProducer::IsInstantiated() {
    return is_successfully_instantiated_;
}
}