#include <array>
#include <iostream>
#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "instrument_symbol.h"
#include "order_outcome_types.h"
#include "order_request_types.h"
#include "trade_order.h"
#include "trade_order_types.h"
#include "trade_quotation_types.h"


class TestTradeOrderConstruction : public ::testing::Test {
    protected:
        std::array<uint8_t, 36> trade_id = {'a', '6', 'b', 'd', '3', 'b', 'a', 'd', '-', '5', '9', \
            '5', 'e', '-', '4', '9', 'c', '1', '-', '9', 'a', '3', 'c', '-', '8', '1', '8', '8', '7', 'e', \
            '1', 'b', '7', '5', '4', '5'};
        std::array<uint8_t, 36> user_id;
        uint32_t instrument_symbol_id = 1;
        TradeOrderType trade_order_type = TradeOrderType::GTC;
        TradeQuotationType trade_quotation_type = TradeQuotationType::BID;
        OrderRequestType order_request_type = OrderRequestType::PLACE_ORDER;
        OrderOutcomeType order_outcome_Type = OrderOutcomeType::NOT_PROCESSED;
        uint64_t price = 100; 
        uint64_t size = 10;
        uint32_t timestamp = 1672647924;
        std::unique_ptr<matchmaker::TradeOrder> trade_order;
        
        void SetUp() override {
            std::fill(user_id.begin(), user_id.end(), 1);
            trade_order = std::make_unique<matchmaker::TradeOrder>(
                trade_id,
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
        }
};

TEST_F(TestTradeOrderConstruction, StandardConstruction) {
    ASSERT_THAT(trade_order->GetTradeId(), ::testing::ContainerEq(trade_id));
    ASSERT_THAT(trade_order->GetUserId(), ::testing::ContainerEq(user_id));
    ASSERT_EQ(trade_order->GetInstrumentSymbolId(), instrument_symbol_id);
    ASSERT_EQ(trade_order->GetOrderType(), trade_order_type);
    ASSERT_EQ(trade_order->GetQuotationType(), trade_quotation_type);
    ASSERT_EQ(trade_order->GetRequestType(), order_request_type);
    ASSERT_EQ(trade_order->GetOrderOutcome(), order_outcome_Type);
    ASSERT_EQ(trade_order->GetPrice(), price);
    ASSERT_EQ(trade_order->GetSize(), size);
    ASSERT_EQ(trade_order->GetTimestamp(), timestamp);
    ASSERT_EQ(trade_order->GetPrevOrder(), nullptr);
    ASSERT_EQ(trade_order->GetNextOrder(), nullptr);
}

TEST_F(TestTradeOrderConstruction, CopyConstructor) {
    matchmaker::TradeOrder copied_trade_order = matchmaker::TradeOrder(*trade_order);
    ASSERT_THAT(copied_trade_order.GetTradeId(), ::testing::ContainerEq(trade_id));
    ASSERT_THAT(copied_trade_order.GetUserId(), ::testing::ContainerEq(user_id));
    ASSERT_EQ(copied_trade_order.GetInstrumentSymbolId(), instrument_symbol_id);
    ASSERT_EQ(copied_trade_order.GetOrderType(), trade_order_type);
    ASSERT_EQ(copied_trade_order.GetQuotationType(), trade_quotation_type);
    ASSERT_EQ(copied_trade_order.GetRequestType(), order_request_type);
    ASSERT_EQ(copied_trade_order.GetOrderOutcome(), order_outcome_Type);
    ASSERT_EQ(copied_trade_order.GetPrice(), price);
    ASSERT_EQ(copied_trade_order.GetSize(), size);
    ASSERT_EQ(copied_trade_order.GetTimestamp(), timestamp);
    ASSERT_EQ(copied_trade_order.GetPrevOrder(), nullptr);
    ASSERT_EQ(copied_trade_order.GetNextOrder(), nullptr);
}

TEST_F(TestTradeOrderConstruction, MoveConstructor) {
    matchmaker::TradeOrder move_constructed_trade_order = matchmaker::TradeOrder(std::move(*trade_order));
    ASSERT_THAT(move_constructed_trade_order.GetTradeId(), ::testing::ContainerEq(trade_id));
    ASSERT_THAT(move_constructed_trade_order.GetUserId(), ::testing::ContainerEq(user_id));
    ASSERT_EQ(move_constructed_trade_order.GetInstrumentSymbolId(), instrument_symbol_id);
    ASSERT_EQ(move_constructed_trade_order.GetOrderType(), trade_order_type);
    ASSERT_EQ(move_constructed_trade_order.GetQuotationType(), trade_quotation_type);
    ASSERT_EQ(move_constructed_trade_order.GetRequestType(), order_request_type);
    ASSERT_EQ(move_constructed_trade_order.GetOrderOutcome(), order_outcome_Type);
    ASSERT_EQ(move_constructed_trade_order.GetPrice(), price);
    ASSERT_EQ(move_constructed_trade_order.GetSize(), size);
    ASSERT_EQ(move_constructed_trade_order.GetTimestamp(), timestamp);
    ASSERT_EQ(move_constructed_trade_order.GetPrevOrder(), nullptr);
    ASSERT_EQ(move_constructed_trade_order.GetNextOrder(), nullptr);
}

TEST_F(TestTradeOrderConstruction, MoveAssignment) {
    matchmaker::TradeOrder move_assigned_trade_order = *trade_order;
    ASSERT_THAT(move_assigned_trade_order.GetTradeId(), ::testing::ContainerEq(trade_id));
    ASSERT_THAT(move_assigned_trade_order.GetUserId(), ::testing::ContainerEq(user_id));
    ASSERT_EQ(move_assigned_trade_order.GetInstrumentSymbolId(), instrument_symbol_id);
    ASSERT_EQ(move_assigned_trade_order.GetOrderType(), trade_order_type);
    ASSERT_EQ(move_assigned_trade_order.GetQuotationType(), trade_quotation_type);
    ASSERT_EQ(move_assigned_trade_order.GetRequestType(), order_request_type);
    ASSERT_EQ(move_assigned_trade_order.GetOrderOutcome(), order_outcome_Type);
    ASSERT_EQ(move_assigned_trade_order.GetPrice(), price);
    ASSERT_EQ(move_assigned_trade_order.GetSize(), size);
    ASSERT_EQ(move_assigned_trade_order.GetTimestamp(), timestamp);
    ASSERT_EQ(move_assigned_trade_order.GetPrevOrder(), nullptr);
    ASSERT_EQ(move_assigned_trade_order.GetNextOrder(), nullptr);
}

TEST_F(TestTradeOrderConstruction, StringifyTradeId) {
    std::string received_trade_id = trade_order->GetTradeIdAsString();
    std::string expected_trade_id = "a6bd3bad-595e-49c1-9a3c-81887e1b7545";
    ASSERT_EQ(received_trade_id, expected_trade_id);
}

TEST(TestInstrumentSymbol, SimpleConstruction) {
    uint32_t instrument_symbol_id = 1;
    matchmaker::InstrumentType instrument_type = matchmaker::InstrumentType::CURRENCY_PAIR;
    int32_t base_currency = 100;
    int32_t quote_currency = 10;
    int32_t base_multiplier_k = 2;
    int32_t quote_multiplier_k = 3;
    int32_t maker_fee = 1;
    matchmaker::InstrumentSymbol eur_usd_pair = matchmaker::InstrumentSymbol(
        instrument_symbol_id,
        instrument_type,
        base_currency,
        quote_currency,
        base_multiplier_k,
        quote_multiplier_k,
        maker_fee
    );
    ASSERT_EQ(eur_usd_pair.GetInstrumentSymbolId(), instrument_symbol_id);
    ASSERT_EQ(eur_usd_pair.GetInstrumentType(), instrument_type);
    ASSERT_EQ(eur_usd_pair.GetBaseCurrency(), base_currency);
    ASSERT_EQ(eur_usd_pair.GetQuoteCurrency(), quote_currency);
    ASSERT_EQ(eur_usd_pair.GetBaseMultiplierK(), base_multiplier_k);
    ASSERT_EQ(eur_usd_pair.GetQuoteMultiplierK(), quote_multiplier_k);
    ASSERT_EQ(eur_usd_pair.GetMakerFee(), maker_fee);
}