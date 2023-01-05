#include <array>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "simple_price_bucket.h"
#include "trade_quotation_types.h"


/**
 * @brief Returns 3 mock bids to be used in testing (all at same price, but at different timestamps and volumes).
 * 
 */
class MockBids : public ::testing::Test {
    private:
        std::array<uint8_t, 36> task_id;
        std::array<uint8_t, 36> user_id;
        uint32_t instrument_symbol_id = 1;
        TradeOrderType trade_order_type = TradeOrderType::GTC;
        TradeQuotationType trade_quotation_type = TradeQuotationType::BID;
        OrderRequestType order_request_type = OrderRequestType::PLACE_ORDER;
        OrderOutcomeType order_outcome_Type = OrderOutcomeType::NOT_PROCESSED;
        
    protected:
        uint64_t price = 100;
        unsigned long int num_mocks_ = 3;
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
                    price,
                    size,
                    timestamp
                );
            }

        }
};


class TestSimplePriceBucketMockOrders : public MockBids {
    protected:
        // void SetUp() override {}
        // void TearDown() override {}
};

TEST(TestSimplePriceBucket, SimpleConstruction) {
    uint64_t price = 10;
    matchmaker::SimplePriceBucket simple_pb = matchmaker::SimplePriceBucket(TradeQuotationType::BID, price);
    ASSERT_EQ(simple_pb.GetPrice(), price);
    ASSERT_EQ(simple_pb.GetTotalVolume(), 0);
    ASSERT_EQ(simple_pb.GetFirstOrder(), nullptr);
}

/**
 * @brief Test for correct price bucket insertion for multiple orders
 * 
 */
TEST_F(TestSimplePriceBucketMockOrders, OrderInsertion) {
    matchmaker::SimplePriceBucket simple_pb = matchmaker::SimplePriceBucket(TradeQuotationType::BID, price);
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
    matchmaker::SimplePriceBucket simple_pb = matchmaker::SimplePriceBucket(TradeQuotationType::BID, 1);
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
    matchmaker::SimplePriceBucket simple_pb = matchmaker::SimplePriceBucket(TradeQuotationType::BID, price);
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
