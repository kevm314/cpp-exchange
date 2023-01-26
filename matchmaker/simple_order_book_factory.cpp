
#include "instrument_symbol.h"
#include "simple_order_book.h"
#include "simple_order_book_factory.h"

namespace matchmaker {

SimpleOrderBookFactory::SimpleOrderBookFactory() {}

std::unique_ptr<matchmaker::BaseOrderBook> SimpleOrderBookFactory::FactoryProduce(matchmaker::InstrumentSymbol* instrument_symbol) {
    return std::make_unique<matchmaker::SimpleOrderBook>(instrument_symbol);
}

}
