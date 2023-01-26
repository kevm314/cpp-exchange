#pragma once

#include "gmock/gmock.h"

#include "simple_order_book_factory.h"

class MockSimpleOrderBookFactory: public matchmaker::SimpleOrderBookFactory {
    public:
        MOCK_METHOD(
            std::unique_ptr<matchmaker::BaseOrderBook>, 
            FactoryProduce, 
            (
                matchmaker::InstrumentSymbol* instrument_symbol
            ), 
            (override)
        );
};