#pragma once

#include <memory>

#include "base_order_book.h"
#include "instrument_symbol.h"

namespace matchmaker {

class OrderBookFactory {
    public:
        OrderBookFactory() {};
        virtual ~OrderBookFactory() = default;
        virtual std::unique_ptr<matchmaker::BaseOrderBook> FactoryProduce(matchmaker::InstrumentSymbol* instrument_symbol) = 0; 
};

}
