#pragma once

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "order_book_factory.h"

namespace matchmaker {

class SimpleOrderBookFactory: public OrderBookFactory {
    public:
        SimpleOrderBookFactory();
        virtual ~SimpleOrderBookFactory() = default;
        virtual std::unique_ptr<matchmaker::BaseOrderBook> FactoryProduce(matchmaker::InstrumentSymbol* instrument_symbol) override; 
};

}
