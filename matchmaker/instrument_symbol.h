#pragma once

#include <stdint.h>
#include <string>

namespace matchmaker {

/**
 * @brief Types of supported financial instruments.
 * 
 */
enum InstrumentType {
    CURRENCY_PAIR = 0,
    FIRST_INSTRUMENT = CURRENCY_PAIR,
    FUTURES_CONTRACT = 1,
    OPTION = 2,
    SHARE = 3,
    LAST_INSTRUMENT = SHARE
};

/**
 * @brief Class for instrument symbols.
 * 
 */
class InstrumentSymbol {
    private:
        const uint32_t instrument_symbol_id_;
        const std::string instrument_name_;
        const InstrumentType instrument_type_;
        const int32_t base_currency_;
        // counter currency
        const int32_t quote_currency_;
        // multiplier in respective currency units
        const int32_t base_multiplier_k_;
        const int32_t quote_multiplier_k_;
        int32_t maker_fee_;
    public:
        InstrumentSymbol(
            uint32_t instrument_symbol_id,
            std::string instrument_name,
            InstrumentType instrument_type,
            uint32_t base_currency,
            uint32_t quote_currency,
            uint32_t base_multiplier_k,
            uint32_t quote_multiplier_k,
            uint32_t maker_fee
        );
        uint32_t GetInstrumentSymbolId() const;        
        InstrumentType GetInstrumentType();
        uint32_t GetBaseCurrency();
        uint32_t GetQuoteCurrency();
        uint32_t GetBaseMultiplierK();
        uint32_t GetQuoteMultiplierK();
        uint32_t GetMakerFee();

};

}
