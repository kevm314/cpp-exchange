#pragma once

#include <stdint.h>
#include <string>

namespace matchmaker {

/**
 * @brief Types of supported financial instruments.
 * 
 */
enum InstrumentType {
    CURRENCY_PAIR,
    FUTURES_CONTRACT,
    OPTION,
    SHARE
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
            int32_t base_currency,
            int32_t quote_currency,
            int32_t base_multiplier_k,
            int32_t quote_multiplier_k,
            int32_t maker_fee
        );
        uint32_t GetInstrumentSymbolId();        
        InstrumentType GetInstrumentType();
        int32_t GetBaseCurrency();
        int32_t GetQuoteCurrency();
        int32_t GetBaseMultiplierK();
        int32_t GetQuoteMultiplierK();
        int32_t GetMakerFee();

};

}
