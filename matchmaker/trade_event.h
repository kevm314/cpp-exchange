#pragma once

#include <array>
#include <string>

namespace matchmaker {

class TradeEvent {
    private:
        const std::array<uint8_t, 36> bid_trade_id_;
        const std::array<uint8_t, 36> ask_trade_id_;
        const uint32_t instrument_symbol_id_;
        const uint64_t price_;
        const uint64_t size_;
        const uint32_t event_timestamp_;
    public:
        TradeEvent(
            std::array<uint8_t, 36> bid_trade_id_,
            std::array<uint8_t, 36> ask_trade_id_,
            uint32_t instrument_symbol_id_,
            uint64_t price_,
            uint64_t size_
        );
        std::array<uint8_t, 36> GetBidTradeId();
        std::array<uint8_t, 36> GetAskTradeId();
        uint32_t GetInstrumentSymbolId();
        uint64_t GetPrice();
        uint64_t GetSize();
        uint32_t GetEventTimestamp();
};

}
