#include <stdint.h>

#include <array>
#include <ctime>

#include "instrument_symbol.h"
#include "trade_event.h"

namespace matchmaker {

TradeEvent::TradeEvent(
    std::array<uint8_t, 36> bid_trade_id,
    std::array<uint8_t, 36> ask_trade_id,
    uint32_t instrument_symbol_id,
    uint64_t price,
    uint64_t size
):
    bid_trade_id_(bid_trade_id),
    ask_trade_id_(ask_trade_id),
    instrument_symbol_id_(instrument_symbol_id),
    price_(price),
    size_(size),
    event_timestamp_(std::time(nullptr))
{}
std::array<uint8_t, 36> TradeEvent::GetBidTradeId() {
    return bid_trade_id_;
}
std::array<uint8_t, 36> TradeEvent::GetAskTradeId() {
    return ask_trade_id_;
}
uint32_t TradeEvent::GetInstrumentSymbolId() {
    return instrument_symbol_id_;
}
uint64_t TradeEvent::GetPrice() {
    return price_;
}
uint64_t TradeEvent::GetSize() {
    return size_;
}
uint32_t TradeEvent::GetEventTimestamp() {
    return event_timestamp_;
}
}