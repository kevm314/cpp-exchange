#include <string>

#include "instrument_symbol.h"


namespace matchmaker {

InstrumentSymbol::InstrumentSymbol(
    uint32_t instrument_symbol_id,
    std::string instrument_name,
    InstrumentType instrument_type,
    uint32_t base_currency,
    uint32_t quote_currency,
    uint32_t base_multiplier_k,
    uint32_t quote_multiplier_k,
    uint32_t maker_fee
):
    instrument_symbol_id_(instrument_symbol_id),
    instrument_name_(instrument_name),
    instrument_type_(instrument_type),
    base_currency_(base_currency),
    quote_currency_(quote_currency),
    base_multiplier_k_(base_multiplier_k),
    quote_multiplier_k_(quote_multiplier_k),
    maker_fee_(maker_fee)
{}

uint32_t InstrumentSymbol::GetInstrumentSymbolId() const {
    return instrument_symbol_id_;
}

InstrumentType InstrumentSymbol::GetInstrumentType() {
    return instrument_type_;
}

uint32_t InstrumentSymbol::GetBaseCurrency() {
    return base_currency_;
}

uint32_t InstrumentSymbol::GetQuoteCurrency() {
    return quote_currency_;
}

uint32_t InstrumentSymbol::GetBaseMultiplierK() {
    return base_multiplier_k_;
}

uint32_t InstrumentSymbol::GetQuoteMultiplierK() {
    return quote_multiplier_k_;
}

uint32_t InstrumentSymbol::GetMakerFee() {
    return maker_fee_;
}

}