#include <stdint.h>

#include <array>
#include <vector>

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "simple_order_book.h"

namespace matchmaker {

SimpleOrderBook::SimpleOrderBook(
    matchmaker::InstrumentSymbol instrument_symbol
):
    BaseOrderBook(instrument_symbol)
{}

}