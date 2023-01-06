#include <array>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "simple_order_book.h"
#include "instrument_symbol.h"
#include "order_outcome_types.h"
#include "order_request_types.h"
#include "trade_order.h"
#include "trade_order_types.h"
#include "trade_quotation_types.h"

class TestSimpleOrderBook : virtual public ::testing::Test {
    protected:
        // void SetUp() override {}
        // void TearDown() override {}
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
};

TEST_F(TestSimpleOrderBook, SimpleConstruction) {
    matchmaker::SimpleOrderBook simple_ob = matchmaker::SimpleOrderBook(eur_usd_pair);
    ASSERT_EQ(simple_ob.GetInstrumentSymbol().GetInstrumentSymbolId(), eur_usd_pair.GetInstrumentSymbolId());
}


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

class TestSimpleOrderBookMockOrders : public TestSimpleOrderBook, public MockBids, public MockAsks {
    protected:
        void SetUp() override {
            TestSimpleOrderBook::SetUp();
            MockBids::SetUp();
            MockAsks::SetUp();
        }
        // void TearDown() override {}
};

// TEST_F(TestSimpleOrderBookMockOrders, PlaceGtc) {
//     matchmaker::SimpleOrderBook simple_ob = matchmaker::SimpleOrderBook(eur_usd_pair);
//     ASSERT_EQ(1, 0);
// }


/**
 * @brief Test price bucket creation and key based querying (competitive price querying).
 * 
 */
TEST_F(TestSimpleOrderBookMockOrders, BidPriceBucketCreationQuerying) {
    TradeQuotationType counter_quote = TradeQuotationType::BID;
    matchmaker::SimpleOrderBook simple_ob = matchmaker::SimpleOrderBook(eur_usd_pair);
    // create 4 price buckets
    simple_ob.CreatePriceBucket(1, counter_quote);
    simple_ob.CreatePriceBucket(2, counter_quote);
    simple_ob.CreatePriceBucket(4, counter_quote);
    simple_ob.CreatePriceBucket(5, counter_quote);
    ASSERT_EQ(simple_ob.GetNumberBuckets(counter_quote), 4);
    matchmaker::PriceBucketsIterator price_buckets = simple_ob.GetCandidatePriceBuckets(3, counter_quote);
    // price buckets 5 then 4
    ASSERT_TRUE(price_buckets.IsIteratorValid());
    ASSERT_EQ(price_buckets.GetCurrentPrice(), 5);
    ASSERT_EQ(price_buckets.GetCurrentBucket().GetPrice(), 5);
    ASSERT_TRUE(price_buckets.IncrementToNextBestPrice());
    ASSERT_EQ(price_buckets.GetCurrentPrice(), 4);
    ASSERT_EQ(price_buckets.GetCurrentBucket().GetPrice(), 4);
    // iterator should no longer be valid after final increment
    ASSERT_FALSE(price_buckets.IncrementToNextBestPrice());
}

/**
 * @brief Test price bucket creation and key based querying (competitive price querying).
 * 
 */
TEST_F(TestSimpleOrderBookMockOrders, BidPriceBucketCreationQueryingNoBuckets) {
    TradeQuotationType counter_quote = TradeQuotationType::BID;
    matchmaker::SimpleOrderBook simple_ob = matchmaker::SimpleOrderBook(eur_usd_pair);
    // create 4 price buckets
    simple_ob.CreatePriceBucket(10, counter_quote);
    simple_ob.CreatePriceBucket(20, counter_quote);
    simple_ob.CreatePriceBucket(40, counter_quote);
    simple_ob.CreatePriceBucket(50, counter_quote);
    ASSERT_EQ(simple_ob.GetNumberBuckets(counter_quote), 4);
    matchmaker::PriceBucketsIterator price_buckets = simple_ob.GetCandidatePriceBuckets(60, counter_quote);
    ASSERT_FALSE(price_buckets.IsIteratorValid());
    // iterator still invalid after final increment
    ASSERT_FALSE(price_buckets.IncrementToNextBestPrice());
}

/**
 * @brief Test price bucket creation and key based querying (competitive price querying).
 * 
 */
TEST_F(TestSimpleOrderBookMockOrders, AskPriceBucketCreationQuerying) {
    TradeQuotationType counter_quote = TradeQuotationType::ASK;
    matchmaker::SimpleOrderBook simple_ob = matchmaker::SimpleOrderBook(eur_usd_pair);
    // create 4 price buckets
    simple_ob.CreatePriceBucket(1, counter_quote);
    simple_ob.CreatePriceBucket(2, counter_quote);
    simple_ob.CreatePriceBucket(4, counter_quote);
    simple_ob.CreatePriceBucket(5, counter_quote);
    ASSERT_EQ(simple_ob.GetNumberBuckets(counter_quote), 4);
    matchmaker::PriceBucketsIterator price_buckets = simple_ob.GetCandidatePriceBuckets(3, counter_quote);
    // price buckets 1 then 2
    ASSERT_TRUE(price_buckets.IsIteratorValid());
    ASSERT_EQ(price_buckets.GetCurrentPrice(), 1);
    ASSERT_EQ(price_buckets.GetCurrentBucket().GetPrice(), 1);
    ASSERT_TRUE(price_buckets.IncrementToNextBestPrice());
    ASSERT_EQ(price_buckets.GetCurrentPrice(), 2);
    ASSERT_EQ(price_buckets.GetCurrentBucket().GetPrice(), 2);
    // iterator should no longer be valid after final increment
    ASSERT_FALSE(price_buckets.IncrementToNextBestPrice());
}

/**
 * @brief Test price bucket creation and key based querying (competitive price querying).
 * 
 */
TEST_F(TestSimpleOrderBookMockOrders, AskPriceBucketCreationQueryingNoBuckets) {
    TradeQuotationType counter_quote = TradeQuotationType::ASK;
    matchmaker::SimpleOrderBook simple_ob = matchmaker::SimpleOrderBook(eur_usd_pair);
    // create 4 price buckets
    simple_ob.CreatePriceBucket(10, counter_quote);
    simple_ob.CreatePriceBucket(20, counter_quote);
    simple_ob.CreatePriceBucket(40, counter_quote);
    simple_ob.CreatePriceBucket(50, counter_quote);
    ASSERT_EQ(simple_ob.GetNumberBuckets(counter_quote), 4);
    matchmaker::PriceBucketsIterator price_buckets = simple_ob.GetCandidatePriceBuckets(3, counter_quote);
    ASSERT_FALSE(price_buckets.IsIteratorValid());
    // iterator still invalid after final increment
    ASSERT_FALSE(price_buckets.IncrementToNextBestPrice());
}