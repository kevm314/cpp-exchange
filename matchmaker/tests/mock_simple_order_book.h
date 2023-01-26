#pragma once

#include "gmock/gmock.h"

#include "simple_order_book.h"

class MockSimpleOrderBook: public matchmaker::SimpleOrderBook {
    public:
        MockSimpleOrderBook(): matchmaker::SimpleOrderBook(nullptr) {}
        // MOCK_METHOD(
        //     OrderOutcomeType, 
        //     ConsumeOrder, 
        //     (
        //         matchmaker::TradeOrder& trade_order, 
        //         std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events
        //     ), 
        //     (override)
        // );
        OrderOutcomeType ConsumeOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) override {
            return OrderOutcomeType::ORDER_CANCEL_SUCCESS;
        }
};