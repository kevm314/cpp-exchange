#include <array>
#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "simple_price_bucket.h"
#include "trade_event.h"
#include "trade_quotation_types.h"


/**
 * @brief Returns 3 mock bids to be used in testing (all at same price, but at different timestamps and volumes).
 * 
 */
class MockBids : virtual public ::testing::Test {
    private:
        std::array<uint8_t, 36> task_id;
        std::array<uint8_t, 36> user_id;
        uint32_t instrument_symbol_id = 1;
        TradeOrderType trade_order_type = TradeOrderType::GTC;
        TradeQuotationType trade_quotation_type = TradeQuotationType::BID;
        OrderRequestType order_request_type = OrderRequestType::PLACE_ORDER;
        OrderOutcomeType order_outcome_Type = OrderOutcomeType::NOT_PROCESSED;
        unsigned long int num_mocks_ = 3;
    protected:
        uint64_t bid_price = 100;
        std::vector<matchmaker::TradeOrder> mock_bids_;
        void SetUp() override {
            mock_bids_.reserve(num_mocks_);
            uint64_t size;
            uint32_t timestamp;
            
            for (unsigned long int index = 0; index < num_mocks_; index++) {
                std::fill(task_id.begin(), task_id.end(), index);
                std::fill(user_id.begin(), user_id.end(), index);
                size = index + 1;
                timestamp = index;

                mock_bids_.emplace(mock_bids_.end(),
                    task_id,
                    user_id,
                    instrument_symbol_id, 
                    trade_order_type,
                    trade_quotation_type,
                    order_request_type,
                    order_outcome_Type,
                    bid_price,
                    size,
                    timestamp
                );
            }

        }
};

/**
 * @brief Returns 3 mock asks to be used in testing (all at same price, but at different timestamps and volumes).
 * 
 */
class MockAsks : virtual public ::testing::Test {
    private:
        std::array<uint8_t, 36> task_id;
        std::array<uint8_t, 36> user_id;
        uint32_t instrument_symbol_id = 1;
        TradeOrderType trade_order_type = TradeOrderType::GTC;
        TradeQuotationType trade_quotation_type = TradeQuotationType::ASK;
        OrderRequestType order_request_type = OrderRequestType::PLACE_ORDER;
        OrderOutcomeType order_outcome_Type = OrderOutcomeType::NOT_PROCESSED;
        unsigned long int num_mocks_ = 3;
        std::array<uint64_t, 3> order_sizes = {1, 4, 7};
    protected:
        uint64_t ask_price = 100;
        std::vector<matchmaker::TradeOrder> mock_asks_;
        
        
        void SetUp() override {
            mock_asks_.reserve(num_mocks_);
            uint64_t size;
            uint32_t timestamp;
            
            for (unsigned long int index = 0; index < num_mocks_; index++) {
                std::fill(task_id.begin(), task_id.end(), index);
                std::fill(user_id.begin(), user_id.end(), index);
                size = order_sizes[index];
                timestamp = index;

                mock_asks_.emplace(mock_asks_.end(),
                    task_id,
                    user_id,
                    instrument_symbol_id, 
                    trade_order_type,
                    trade_quotation_type,
                    order_request_type,
                    order_outcome_Type,
                    ask_price,
                    size,
                    timestamp
                );
            }

        }
};

class TestSimplePriceBucketMockOrders : public MockBids, public MockAsks {
    protected:
        void SetUp() override {
            MockBids::SetUp();
            MockAsks::SetUp();
        }
        // void TearDown() override {}
};

TEST(TestSimplePriceBucket, SimpleConstruction) {
    uint64_t price = 10;
    matchmaker::SimplePriceBucket simple_pb = matchmaker::SimplePriceBucket(TradeQuotationType::BID, 1, price);
    ASSERT_EQ(simple_pb.GetPrice(), price);
    ASSERT_EQ(simple_pb.GetTotalVolume(), 0);
    ASSERT_EQ(simple_pb.GetFirstOrder(), nullptr);
}

/**
 * @brief Test for correct price bucket insertion for multiple orders
 * 
 */
TEST_F(TestSimplePriceBucketMockOrders, OrderInsertion) {
    matchmaker::SimplePriceBucket simple_pb = matchmaker::SimplePriceBucket(TradeQuotationType::BID, 1, bid_price);
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[0]));
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[1]));
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[2]));
    ASSERT_EQ(mock_bids_[0].GetPrevOrder(), nullptr);
    ASSERT_EQ(mock_bids_[0].GetNextOrder(), &mock_bids_[1]);
    ASSERT_EQ(mock_bids_[1].GetPrevOrder(), &mock_bids_[0]);
    ASSERT_EQ(mock_bids_[1].GetNextOrder(), &mock_bids_[2]);
    ASSERT_EQ(mock_bids_[2].GetPrevOrder(), &mock_bids_[1]);
    ASSERT_EQ(mock_bids_[2].GetNextOrder(), nullptr);
    ASSERT_EQ(
        simple_pb.GetTotalVolume(), 
        mock_bids_[0].GetSize() + mock_bids_[1].GetSize() + mock_bids_[2].GetSize()
    );
    ASSERT_TRUE((simple_pb.IsOrderInserted(mock_bids_[0])));
    ASSERT_TRUE((simple_pb.IsOrderInserted(mock_bids_[1])));
    ASSERT_TRUE((simple_pb.IsOrderInserted(mock_bids_[2])));
    ASSERT_EQ(simple_pb.GetFirstOrder(), &mock_bids_[0]);
    ASSERT_EQ(simple_pb.GetLastOrder(), &mock_bids_[2]);
}

/**
 * @brief Test for incorrect price bucket insertion.
 * 
 */
TEST_F(TestSimplePriceBucketMockOrders, BadOrderInsertion) {
    matchmaker::SimplePriceBucket simple_pb = matchmaker::SimplePriceBucket(TradeQuotationType::BID, 1, 1);
    ASSERT_FALSE(simple_pb.InsertOrder(mock_bids_[0]));
    ASSERT_FALSE(simple_pb.InsertOrder(mock_bids_[1]));
    ASSERT_FALSE(simple_pb.InsertOrder(mock_bids_[2]));
    ASSERT_EQ(mock_bids_[0].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[1].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[2].GetNextOrder(), nullptr);
    ASSERT_EQ(
        simple_pb.GetTotalVolume(), 
        0
    );
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[0])));
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[1])));
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[2])));
}

/**
 * @brief Test for correct price bucket deletion for single order amongst many.
 * 
 */
TEST_F(TestSimplePriceBucketMockOrders, OrderDeletion) {
    // Set up price bucket and insert mock bids
    matchmaker::SimplePriceBucket simple_pb = matchmaker::SimplePriceBucket(TradeQuotationType::BID, 1, bid_price);
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[0]));
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[1]));
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[2]));

    // Remove middle element then check volume and list
    ASSERT_TRUE(simple_pb.EraseOrder(mock_bids_[1]));
    ASSERT_EQ(
        simple_pb.GetTotalVolume(), 
        mock_bids_[0].GetSize() + mock_bids_[2].GetSize()
    );
    ASSERT_EQ(mock_bids_[0].GetNextOrder(), &mock_bids_[2]);
    ASSERT_EQ(mock_bids_[2].GetPrevOrder(), &mock_bids_[0]);
    ASSERT_EQ(mock_bids_[1].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[1].GetPrevOrder(), nullptr);
    ASSERT_TRUE((simple_pb.IsOrderInserted(mock_bids_[0])));
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[1])));
    ASSERT_TRUE((simple_pb.IsOrderInserted(mock_bids_[2])));

    // Remove head then check ...
    ASSERT_TRUE(simple_pb.EraseOrder(mock_bids_[0]));
    ASSERT_EQ(
        simple_pb.GetTotalVolume(), 
        mock_bids_[2].GetSize()
    );
    ASSERT_EQ(mock_bids_[0].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[0].GetPrevOrder(), nullptr);
    ASSERT_EQ(mock_bids_[1].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[1].GetPrevOrder(), nullptr);
    ASSERT_EQ(mock_bids_[2].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[2].GetPrevOrder(), nullptr);
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[0])));
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[1])));
    ASSERT_TRUE((simple_pb.IsOrderInserted(mock_bids_[2])));

    // Remove single element then check ...
    ASSERT_TRUE(simple_pb.EraseOrder(mock_bids_[2]));
    ASSERT_EQ(
        simple_pb.GetTotalVolume(), 
        0
    );
    ASSERT_EQ(mock_bids_[0].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[0].GetPrevOrder(), nullptr);
    ASSERT_EQ(mock_bids_[1].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[1].GetPrevOrder(), nullptr);
    ASSERT_EQ(mock_bids_[2].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[2].GetPrevOrder(), nullptr);
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[0])));
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[1])));
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[2])));
}

/**
 * @brief Test for basic order filling (1 ask against 1 bid - 1 ask, 1 bid completely filled)
 * 
 */
TEST_F(TestSimplePriceBucketMockOrders, OrderFilling1v1) {
    matchmaker::SimplePriceBucket simple_pb = matchmaker::SimplePriceBucket(TradeQuotationType::BID, 1, bid_price);
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[0]));
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[1]));
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[2]));
    // test simple ask order completely filled
    std::vector<std::string> trades_to_remove;
    std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events = std::make_shared<std::vector<matchmaker::TradeEvent>>();
    ASSERT_EQ(simple_pb.FulfillOrder(mock_asks_[0], trade_events, trades_to_remove), 1);
    ASSERT_EQ(mock_bids_[0].GetFilled(), mock_bids_[0].GetSize());
    // test remaining bid orders
    ASSERT_EQ(
        simple_pb.GetTotalVolume(),
        mock_bids_[1].GetSize() + mock_bids_[2].GetSize()
    );
    ASSERT_EQ(mock_bids_[1].GetNextOrder(), &mock_bids_[2]);
    ASSERT_EQ(mock_bids_[2].GetPrevOrder(), &mock_bids_[1]);
    ASSERT_EQ(mock_bids_[0].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[0].GetPrevOrder(), nullptr);
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[0])));
    ASSERT_TRUE((simple_pb.IsOrderInserted(mock_bids_[1])));
    ASSERT_TRUE((simple_pb.IsOrderInserted(mock_bids_[2])));
    // check trade events
    // first trade
    ASSERT_EQ(trade_events->size(), 1);
    ASSERT_EQ(trades_to_remove.size(), 1);
    ASSERT_EQ((*trade_events)[0].GetBidTradeId(), mock_bids_[0].GetTradeId());
    ASSERT_EQ((*trade_events)[0].GetAskTradeId(), mock_asks_[0].GetTradeId());
    ASSERT_EQ(trades_to_remove[0], mock_bids_[0].GetTradeIdAsString());
    ASSERT_EQ((*trade_events)[0].GetSize(), 1);
} 

/**
 * @brief Test for order filling (1 ask against 3 bids - 1 ask, 2 bids completely filled, 1 bid partially filled)
 * 
 */
TEST_F(TestSimplePriceBucketMockOrders, OrderFilling1v3) {
    matchmaker::SimplePriceBucket simple_pb = matchmaker::SimplePriceBucket(TradeQuotationType::BID, 1, bid_price);
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[0]));
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[1]));
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[2]));
    // test simple ask order completely filled
    std::vector<std::string> trades_to_remove;
    std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events = std::make_shared<std::vector<matchmaker::TradeEvent>>();
    ASSERT_EQ(simple_pb.FulfillOrder(mock_asks_[1], trade_events, trades_to_remove), 4);
    ASSERT_EQ(mock_bids_[0].GetFilled(), mock_bids_[0].GetSize());
    ASSERT_EQ(mock_bids_[1].GetFilled(), mock_bids_[1].GetSize());
    // test remaining bid orders
    ASSERT_EQ(
        simple_pb.GetTotalVolume(),
        mock_bids_[2].GetSize() - 1
    );
    ASSERT_EQ(mock_bids_[0].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[0].GetPrevOrder(), nullptr);
    ASSERT_EQ(mock_bids_[1].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[1].GetPrevOrder(), nullptr);
    ASSERT_EQ(mock_bids_[2].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[2].GetPrevOrder(), nullptr);
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[0])));
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[1])));
    ASSERT_TRUE((simple_pb.IsOrderInserted(mock_bids_[2])));
    // check trade events
    ASSERT_EQ(trade_events->size(), 3);
    ASSERT_EQ(trades_to_remove.size(), 2);
    // first trade
    ASSERT_EQ((*trade_events)[0].GetBidTradeId(), mock_bids_[0].GetTradeId());
    ASSERT_EQ((*trade_events)[0].GetAskTradeId(), mock_asks_[1].GetTradeId());
    ASSERT_EQ(trades_to_remove[0], mock_bids_[0].GetTradeIdAsString());
    ASSERT_EQ((*trade_events)[0].GetSize(), 1);
    // second trade
    ASSERT_EQ((*trade_events)[1].GetBidTradeId(), mock_bids_[1].GetTradeId());
    ASSERT_EQ((*trade_events)[1].GetAskTradeId(), mock_asks_[1].GetTradeId());
    ASSERT_EQ(trades_to_remove[1], mock_bids_[1].GetTradeIdAsString());
    ASSERT_EQ((*trade_events)[1].GetSize(), 2);
    // third trade
    ASSERT_EQ((*trade_events)[2].GetBidTradeId(), mock_bids_[2].GetTradeId());
    ASSERT_EQ((*trade_events)[2].GetAskTradeId(), mock_asks_[1].GetTradeId());
    ASSERT_EQ((*trade_events)[2].GetSize(), 1);

}

/**
 * @brief Test for order filling (1 ask against 3 bids - 3 bids completely filled, 1 ask partially filled)
 * 
 */
TEST_F(TestSimplePriceBucketMockOrders, OrderPartialFilling1v3) {
    matchmaker::SimplePriceBucket simple_pb = matchmaker::SimplePriceBucket(TradeQuotationType::BID, 1, bid_price);
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[0]));
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[1]));
    ASSERT_TRUE(simple_pb.InsertOrder(mock_bids_[2]));
    // test simple ask order completely filled
    std::vector<std::string> trades_to_remove;
    std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events = std::make_shared<std::vector<matchmaker::TradeEvent>>();
    ASSERT_EQ(simple_pb.FulfillOrder(mock_asks_[2], trade_events, trades_to_remove), 6);
    ASSERT_EQ(mock_bids_[0].GetFilled(), mock_bids_[0].GetSize());
    ASSERT_EQ(mock_bids_[1].GetFilled(), mock_bids_[1].GetSize());
    ASSERT_EQ(mock_bids_[2].GetFilled(), mock_bids_[2].GetSize());
    // test remaining bid orders
    ASSERT_EQ(
        simple_pb.GetTotalVolume(),
        0
    );
    ASSERT_EQ(mock_bids_[0].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[0].GetPrevOrder(), nullptr);
    ASSERT_EQ(mock_bids_[1].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[1].GetPrevOrder(), nullptr);
    ASSERT_EQ(mock_bids_[2].GetNextOrder(), nullptr);
    ASSERT_EQ(mock_bids_[2].GetPrevOrder(), nullptr);
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[0])));
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[1])));
    ASSERT_FALSE((simple_pb.IsOrderInserted(mock_bids_[2])));
    // check trade events
    ASSERT_EQ(trade_events->size(), 3);
    ASSERT_EQ(trades_to_remove.size(), 3);
    // first trade
    ASSERT_EQ((*trade_events)[0].GetBidTradeId(), mock_bids_[0].GetTradeId());
    ASSERT_EQ((*trade_events)[0].GetAskTradeId(), mock_asks_[2].GetTradeId());
    ASSERT_EQ(trades_to_remove[0], mock_bids_[0].GetTradeIdAsString());
    ASSERT_EQ((*trade_events)[0].GetSize(), 1);
    // second trade
    ASSERT_EQ((*trade_events)[1].GetBidTradeId(), mock_bids_[1].GetTradeId());
    ASSERT_EQ((*trade_events)[1].GetAskTradeId(), mock_asks_[2].GetTradeId());
    ASSERT_EQ(trades_to_remove[1], mock_bids_[1].GetTradeIdAsString());
    ASSERT_EQ((*trade_events)[1].GetSize(), 2);
    // third trade
    ASSERT_EQ((*trade_events)[2].GetBidTradeId(), mock_bids_[2].GetTradeId());
    ASSERT_EQ((*trade_events)[2].GetAskTradeId(), mock_asks_[2].GetTradeId());
    ASSERT_EQ(trades_to_remove[2], mock_bids_[2].GetTradeIdAsString());
    ASSERT_EQ((*trade_events)[2].GetSize(), 3);
}