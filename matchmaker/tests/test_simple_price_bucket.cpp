#include <array>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "simple_price_bucket.h"


/**
 * @brief Returns 3 mock bids to be used in testing (all at same price, but at different timestamps and volumes).
 * 
 */
class MockBids : public ::testing::Test {
    private:
        std::array<uint8_t, 32> task_id;
        std::array<uint8_t, 32> user_id;
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
                size = index;
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
    matchmaker::SimplePriceBucket simple_pb = matchmaker::SimplePriceBucket(price);
    ASSERT_EQ(simple_pb.GetPrice(), price);
    ASSERT_EQ(simple_pb.GetTotalVolume(), 0);
    ASSERT_EQ(simple_pb.GetFirstOrder(), nullptr);
}

TEST_F(MockBids, OrderInsertion) {
    matchmaker::SimplePriceBucket simple_pb = matchmaker::SimplePriceBucket(price);
    ASSERT_EQ(1, 1);
}