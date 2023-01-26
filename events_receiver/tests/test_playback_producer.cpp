#include <filesystem>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "playback_producer.h"
#include "simple_order_book.h"
#include "instrument_symbol.h"
#include "order_outcome_types.h"
#include "order_request_types.h"
#include "trade_order.h"
#include "trade_order_types.h"
#include "trade_quotation_types.h"

class TestPlaybackProducer : public ::testing::Test {
    protected:
        // void SetUp() override {}
        // void TearDown() override {}
        #ifdef MAIN_TEST_BUILD
        std::string playback_path = "../events_receiver/data/three_users_playback.csv";
        #else
        std::string playback_path = "../data/three_users_playback.csv";
        #endif
        
};

TEST_F(TestPlaybackProducer, SimpleConstructionAndPlayback) {
    events_receiver::PlaybackProducer bp = events_receiver::PlaybackProducer(1, playback_path);
    ASSERT_TRUE(bp.IsInstantiated());
    for (unsigned int long i = 0; i < 3; i++) {
        matchmaker::TradeOrder received_trade_order = bp.ReceiveEvent();
        ASSERT_EQ(received_trade_order.GetTradeIdAsString(), "61d66b88-fef8-4ea4-924b-e0d3528ad78d");
        ASSERT_EQ(received_trade_order.GetUserIdAsString(), "cfdb430b-0bc6-4d7c-a024-88d0af5a9b1d");
        ASSERT_EQ(received_trade_order.GetInstrumentSymbolId(), 0);
        ASSERT_EQ(received_trade_order.GetOrderType(), TradeOrderType::GTC);
        ASSERT_EQ(received_trade_order.GetQuotationType(), TradeQuotationType::BID);
        ASSERT_EQ(received_trade_order.GetRequestType(), OrderRequestType::PLACE_ORDER);
        ASSERT_EQ(received_trade_order.GetOrderOutcome(), OrderOutcomeType::NOT_PROCESSED);
        ASSERT_EQ(received_trade_order.GetPrice(), 132);
        ASSERT_EQ(received_trade_order.GetSize(), 1000);
        ASSERT_EQ(received_trade_order.GetTimestamp(), 1);

        received_trade_order = bp.ReceiveEvent();
        ASSERT_EQ(received_trade_order.GetTradeIdAsString(), "46ac997e-e588-413c-af6a-22df294795f0");
        ASSERT_EQ(received_trade_order.GetUserIdAsString(), "cfdb430b-0bc6-4d7c-a024-88d0af5a9b1d");
        ASSERT_EQ(received_trade_order.GetInstrumentSymbolId(), 0);
        ASSERT_EQ(received_trade_order.GetOrderType(), TradeOrderType::GTC);
        ASSERT_EQ(received_trade_order.GetQuotationType(), TradeQuotationType::ASK);
        ASSERT_EQ(received_trade_order.GetRequestType(), OrderRequestType::PLACE_ORDER);
        ASSERT_EQ(received_trade_order.GetOrderOutcome(), OrderOutcomeType::NOT_PROCESSED);
        ASSERT_EQ(received_trade_order.GetPrice(), 132);
        ASSERT_EQ(received_trade_order.GetSize(), 1000);
        ASSERT_EQ(received_trade_order.GetTimestamp(), 2);
        // test wrap around on events stream of size 1
    }
}