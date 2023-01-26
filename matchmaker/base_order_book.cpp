#include <stdint.h>

#include <array>
#include <memory>
#include <vector>

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "trade_event.h"

namespace matchmaker {

BaseOrderBook::BaseOrderBook(
    matchmaker::InstrumentSymbol* instrument_symbol
):
    instrument_symbol_(instrument_symbol)
{}
const matchmaker::InstrumentSymbol* BaseOrderBook::GetInstrumentSymbol() {
    return instrument_symbol_;
}

}
