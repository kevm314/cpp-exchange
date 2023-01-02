#include <array>
#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "order_outcome_types.h"
#include "order_request_types.h"
#include "trade_order.h"
#include "trade_order_types.h"
#include "trade_quotation_types.h"

TEST(TestTradeOrder, SimpleConstruction) {
    std::array<uint8_t, 32> task_id;
    std::fill(task_id.begin(), task_id.end(), 1);
    std::array<uint8_t, 32> user_id;
    std::fill(user_id.begin(), user_id.end(), 1);
    uint32_t instrument_symbol_id = 1;
    TradeOrderType trade_order_type = TradeOrderType::GTC;
    TradeQuotationType trade_quotation_type = TradeQuotationType::BID;
    OrderRequestType order_request_type = OrderRequestType::PLACE_ORDER;
    OrderOutcomeType order_outcome_Type = OrderOutcomeType::NOT_PROCESSED;
    uint64_t price = 100; 
    uint64_t size = 10;
    uint32_t timestamp = 1672647924;
    matchmaker::TradeOrder trade_order = matchmaker::TradeOrder(
        task_id,
        user_id,
        instrument_symbol_id, 
        trade_order_type,
        trade_quotation_type,
        order_request_type,
        order_outcome_Type,
        price,
        size,
        timestamp
    );
    ASSERT_THAT(trade_order.GetTaskId(), ::testing::ContainerEq(task_id));
    ASSERT_THAT(trade_order.GetUserId(), ::testing::ContainerEq(user_id));
    ASSERT_EQ(trade_order.GetInstrumentSymbolId(), instrument_symbol_id);
    ASSERT_EQ(trade_order.GetOrderType(), trade_order_type);
    ASSERT_EQ(trade_order.GetQuotationType(), trade_quotation_type);
    ASSERT_EQ(trade_order.GetRequestType(), order_request_type);
    ASSERT_EQ(trade_order.GetOrderOutcome(), order_outcome_Type);
    ASSERT_EQ(trade_order.GetPrice(), price);
    ASSERT_EQ(trade_order.GetSize(), size);
    ASSERT_EQ(trade_order.GetTimestamp(), timestamp);
}