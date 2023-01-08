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
            "test_symbol",
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
class MockBidsOb : virtual public ::testing::Test {
    private:
        std::array<uint8_t, 36> task_id;
        std::array<uint8_t, 36> user_id;
        uint32_t instrument_symbol_id = 1;
        TradeOrderType trade_order_type = TradeOrderType::GTC;
        TradeQuotationType trade_quotation_type = TradeQuotationType::BID;
        OrderRequestType order_request_type = OrderRequestType::PLACE_ORDER;
        OrderOutcomeType order_outcome_Type = OrderOutcomeType::NOT_PROCESSED;
        unsigned long int num_mocks_ = 3;
        std::array<uint64_t, 3> order_sizes = {1, 2, 3};
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
                size = order_sizes[index];
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
 * @brief Returns 3 mock Ioc bids to be used in testing (all at same price, but at different timestamps and volumes).
 * 
 */
class MockBidsIocOb : virtual public ::testing::Test {
    private:
        std::array<uint8_t, 36> task_id;
        std::array<uint8_t, 36> user_id;
        uint32_t instrument_symbol_id = 1;
        TradeOrderType trade_order_type = TradeOrderType::IOC;
        TradeQuotationType trade_quotation_type = TradeQuotationType::BID;
        OrderRequestType order_request_type = OrderRequestType::PLACE_ORDER;
        OrderOutcomeType order_outcome_Type = OrderOutcomeType::NOT_PROCESSED;
        unsigned long int num_mocks_ = 3;
        std::array<uint64_t, 3> order_sizes = {3, 4, 7};
    protected:
        uint64_t bid_ioc_price = 100;
        std::vector<matchmaker::TradeOrder> mock_bids_ioc_;
        void SetUp() override {
            mock_bids_ioc_.reserve(num_mocks_);
            uint64_t size;
            uint32_t timestamp;
            
            for (unsigned long int index = 0; index < num_mocks_; index++) {
                std::fill(task_id.begin(), task_id.end(), index + 100);
                std::fill(user_id.begin(), user_id.end(), index + 100);
                size = order_sizes[index];
                timestamp = index;

                mock_bids_ioc_.emplace(mock_bids_ioc_.end(),
                    task_id,
                    user_id,
                    instrument_symbol_id, 
                    trade_order_type,
                    trade_quotation_type,
                    order_request_type,
                    order_outcome_Type,
                    bid_ioc_price,
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
class MockAsksOb : virtual public ::testing::Test {
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
                std::fill(task_id.begin(), task_id.end(), index * 4);
                std::fill(user_id.begin(), user_id.end(), index * 4);
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

class TestSimpleOrderBookMockOrders : public TestSimpleOrderBook, public MockBidsOb, public MockAsksOb, public MockBidsIocOb {
    protected:
        void SetUp() override {
            TestSimpleOrderBook::SetUp();
            MockBidsOb::SetUp();
            MockBidsIocOb::SetUp();
            MockAsksOb::SetUp();
        }
        // void TearDown() override {}
};


/**
 * @brief Test price bucket creation and key based querying (competitive price querying).
 * 
 */
TEST_F(TestSimpleOrderBookMockOrders, BidPriceBucketCreationQuerying) {
    TradeQuotationType counter_quote = TradeQuotationType::BID;
    matchmaker::SimpleOrderBook simple_ob = matchmaker::SimpleOrderBook(eur_usd_pair);
    // create 4 price buckets
    ASSERT_TRUE(simple_ob.CreatePriceBucket(1, counter_quote));
    ASSERT_TRUE(simple_ob.CreatePriceBucket(2, counter_quote));
    ASSERT_TRUE(simple_ob.CreatePriceBucket(4, counter_quote));
    ASSERT_TRUE(simple_ob.CreatePriceBucket(5, counter_quote));
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
    ASSERT_TRUE(simple_ob.CreatePriceBucket(10, counter_quote));
    ASSERT_TRUE(simple_ob.CreatePriceBucket(20, counter_quote));
    ASSERT_TRUE(simple_ob.CreatePriceBucket(40, counter_quote));
    ASSERT_TRUE(simple_ob.CreatePriceBucket(50, counter_quote));
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
    ASSERT_TRUE(simple_ob.CreatePriceBucket(1, counter_quote));
    ASSERT_TRUE(simple_ob.CreatePriceBucket(2, counter_quote));
    ASSERT_TRUE(simple_ob.CreatePriceBucket(4, counter_quote));
    ASSERT_TRUE(simple_ob.CreatePriceBucket(5, counter_quote));
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
    ASSERT_TRUE(simple_ob.CreatePriceBucket(10, counter_quote));
    ASSERT_TRUE(simple_ob.CreatePriceBucket(20, counter_quote));
    ASSERT_TRUE(simple_ob.CreatePriceBucket(40, counter_quote));
    ASSERT_TRUE(simple_ob.CreatePriceBucket(50, counter_quote));
    ASSERT_EQ(simple_ob.GetNumberBuckets(counter_quote), 4);
    matchmaker::PriceBucketsIterator price_buckets = simple_ob.GetCandidatePriceBuckets(3, counter_quote);
    ASSERT_FALSE(price_buckets.IsIteratorValid());
    // iterator still invalid after final increment
    ASSERT_FALSE(price_buckets.IncrementToNextBestPrice());
}

/**
 * @brief Mock GTC order scenarios, 2 bids, then 1 ask, then 1 bid
 * 
 */
TEST_F(TestSimpleOrderBookMockOrders, PlaceGtcSimple) {
    std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events = std::make_shared<std::vector<matchmaker::TradeEvent>>();
    matchmaker::SimpleOrderBook simple_ob = matchmaker::SimpleOrderBook(eur_usd_pair);
    // first bid
    ASSERT_EQ(simple_ob.ConsumeOrder(mock_bids_[0], trade_events), OrderOutcomeType::ORDER_NOT_FILLED);
    ASSERT_EQ((*trade_events).size(), 0);
    ASSERT_TRUE(simple_ob.DoesTradeExist(mock_bids_[0].GetTradeIdAsString()));
    // second bid
    ASSERT_EQ(simple_ob.ConsumeOrder(mock_bids_[1], trade_events), OrderOutcomeType::ORDER_NOT_FILLED);
    ASSERT_EQ((*trade_events).size(), 0);
    ASSERT_TRUE(simple_ob.DoesTradeExist(mock_bids_[0].GetTradeIdAsString()));
    ASSERT_TRUE(simple_ob.DoesTradeExist(mock_bids_[1].GetTradeIdAsString()));
    // first ask (2 bids completely filled hence 2 trade events of respective volume 1, 2)
    ASSERT_EQ(simple_ob.ConsumeOrder(mock_asks_[1], trade_events), OrderOutcomeType::ORDER_PARTIALLY_FILLED);
    ASSERT_EQ((*trade_events).size(), 2);
    ASSERT_FALSE(simple_ob.DoesTradeExist(mock_bids_[0].GetTradeIdAsString()));
    ASSERT_FALSE(simple_ob.DoesTradeExist(mock_bids_[1].GetTradeIdAsString()));
    ASSERT_TRUE(simple_ob.DoesTradeExist(mock_asks_[1].GetTradeIdAsString()));
    ASSERT_EQ((*trade_events)[0].GetBidTradeId(), mock_bids_[0].GetTradeId());
    ASSERT_EQ((*trade_events)[0].GetAskTradeId(), mock_asks_[1].GetTradeId());
    ASSERT_EQ((*trade_events)[0].GetSize(), 1);
    ASSERT_EQ((*trade_events)[1].GetBidTradeId(), mock_bids_[1].GetTradeId());
    ASSERT_EQ((*trade_events)[1].GetAskTradeId(), mock_asks_[1].GetTradeId());
    ASSERT_EQ((*trade_events)[1].GetSize(), 2);
    OrderOutcomeType find_trade_outcome;
    matchmaker::TradeOrder* second_ask = simple_ob.FindMatchingTradeOrder(mock_asks_[1], find_trade_outcome);
    ASSERT_EQ(second_ask->GetFilled(), 3);
    // third bid (1 trade event of volume 1)
    ASSERT_EQ(simple_ob.ConsumeOrder(mock_bids_[2], trade_events), OrderOutcomeType::ORDER_PARTIALLY_FILLED);
    ASSERT_EQ((*trade_events).size(), 3);
    ASSERT_TRUE(simple_ob.DoesTradeExist(mock_bids_[2].GetTradeIdAsString()));
    ASSERT_FALSE(simple_ob.DoesTradeExist(mock_asks_[1].GetTradeIdAsString()));
    ASSERT_EQ((*trade_events)[2].GetBidTradeId(), mock_bids_[2].GetTradeId());
    ASSERT_EQ((*trade_events)[2].GetAskTradeId(), mock_asks_[1].GetTradeId());
    ASSERT_EQ((*trade_events)[2].GetSize(), 1);
    matchmaker::TradeOrder* third_bid = simple_ob.FindMatchingTradeOrder(mock_bids_[2], find_trade_outcome);
    ASSERT_EQ(third_bid->GetFilled(), 1);
}

/**
 * @brief Mock IOC and GTC order scenarios, 1 GTC Ask, then 1 IOC Bid
 * 
 */
TEST_F(TestSimpleOrderBookMockOrders, PlaceGtcIoc) {
    std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events = std::make_shared<std::vector<matchmaker::TradeEvent>>();
    matchmaker::SimpleOrderBook simple_ob = matchmaker::SimpleOrderBook(eur_usd_pair);
    // first GTC ask
    ASSERT_EQ(simple_ob.ConsumeOrder(mock_asks_[0], trade_events), OrderOutcomeType::ORDER_NOT_FILLED);
    ASSERT_EQ((*trade_events).size(), 0);
    ASSERT_TRUE(simple_ob.DoesTradeExist(mock_asks_[0].GetTradeIdAsString()));
    // second IOC bid
    ASSERT_EQ(simple_ob.ConsumeOrder(mock_bids_ioc_[0], trade_events), OrderOutcomeType::ORDER_PARTIALLY_FILLED);
    ASSERT_EQ((*trade_events).size(), 1);
    ASSERT_FALSE(simple_ob.DoesTradeExist(mock_asks_[0].GetTradeIdAsString()));
    ASSERT_FALSE(simple_ob.DoesTradeExist(mock_bids_ioc_[0].GetTradeIdAsString()));
    ASSERT_EQ((*trade_events)[0].GetBidTradeId(), mock_bids_ioc_[0].GetTradeId());
    ASSERT_EQ((*trade_events)[0].GetAskTradeId(), mock_asks_[0].GetTradeId());
    ASSERT_EQ((*trade_events)[0].GetSize(), 1);
}


/**
 * @brief Test alter order price.
 * 
 */
TEST_F(TestSimpleOrderBookMockOrders, AlterOrderPrice) {
    std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events = std::make_shared<std::vector<matchmaker::TradeEvent>>();
    matchmaker::SimpleOrderBook simple_ob = matchmaker::SimpleOrderBook(eur_usd_pair);
    uint64_t old_price = mock_bids_[0].GetPrice();
    // place order
    ASSERT_EQ(simple_ob.ConsumeOrder(mock_bids_[0], trade_events), OrderOutcomeType::ORDER_NOT_FILLED);
    // check it exists
    ASSERT_TRUE(simple_ob.DoesTradeExist(mock_bids_[0].GetTradeIdAsString()));
    // confirm number of orders in specific price bucket
    ASSERT_EQ(simple_ob.GetNumOrdersAtPrice(mock_bids_[0].GetPrice(), mock_bids_[0].GetQuotationType()), 1);
    // alter order price
    matchmaker::TradeOrder new_price_copy = matchmaker::TradeOrder(mock_bids_[0]);
    ASSERT_TRUE(new_price_copy.SetNewPrice(200));
    ASSERT_EQ(simple_ob.AlterOrderPrice(new_price_copy, trade_events), OrderOutcomeType::ORDER_NOT_FILLED);
    // check order still exists
    ASSERT_TRUE(simple_ob.DoesTradeExist(mock_bids_[0].GetTradeIdAsString()));
    // confirm number of orders in specific price bucket
    ASSERT_EQ(simple_ob.GetNumOrdersAtPrice(old_price, new_price_copy.GetQuotationType()), 0);
    ASSERT_EQ(simple_ob.GetNumOrdersAtPrice(new_price_copy.GetPrice(), new_price_copy.GetQuotationType()), 1);
}


/**
 * @brief Test alter order size.
 * 
 */
TEST_F(TestSimpleOrderBookMockOrders, AlterOrderSize) {
    std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events = std::make_shared<std::vector<matchmaker::TradeEvent>>();
    matchmaker::SimpleOrderBook simple_ob = matchmaker::SimpleOrderBook(eur_usd_pair);
    // place order
    ASSERT_EQ(simple_ob.ConsumeOrder(mock_bids_[0], trade_events), OrderOutcomeType::ORDER_NOT_FILLED);
    // check it exists
    ASSERT_TRUE(simple_ob.DoesTradeExist(mock_bids_[0].GetTradeIdAsString()));
    // confirm volume in specific price bucket
    ASSERT_EQ(simple_ob.GetVolumeAtPrice(mock_bids_[0].GetPrice(), mock_bids_[0].GetQuotationType()), mock_bids_[0].GetSize());
    // alter order size
    matchmaker::TradeOrder new_size_copy = matchmaker::TradeOrder(mock_bids_[0]);
    uint64_t new_size = 101;
    ASSERT_TRUE(new_size_copy.SetNewSize(new_size));
    ASSERT_EQ(simple_ob.AlterOrderSize(new_size_copy, trade_events), OrderOutcomeType::SUCCESS);
    // check order still exists
    ASSERT_TRUE(simple_ob.DoesTradeExist(mock_bids_[0].GetTradeIdAsString()));
    // confirm size in specific price bucket
    ASSERT_EQ(simple_ob.GetVolumeAtPrice(new_size_copy.GetPrice(), new_size_copy.GetQuotationType()), new_size);
}

/**
 * @brief Test cancel order.
 * 
 */
TEST_F(TestSimpleOrderBookMockOrders, CancelOrder) {
    std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events = std::make_shared<std::vector<matchmaker::TradeEvent>>();
    matchmaker::SimpleOrderBook simple_ob = matchmaker::SimpleOrderBook(eur_usd_pair);
    // place order
    ASSERT_EQ(simple_ob.ConsumeOrder(mock_bids_[0], trade_events), OrderOutcomeType::ORDER_NOT_FILLED);
    // check it exists
    ASSERT_TRUE(simple_ob.DoesTradeExist(mock_bids_[0].GetTradeIdAsString()));
    // confirm volume in specific price bucket
    ASSERT_EQ(simple_ob.GetVolumeAtPrice(mock_bids_[0].GetPrice(), mock_bids_[0].GetQuotationType()), mock_bids_[0].GetSize());
    // cancel order
    ASSERT_EQ(simple_ob.CancelOrder(mock_bids_[0], trade_events), OrderOutcomeType::ORDER_CANCEL_SUCCESS);
    // check it no longer exists and price volume is 0
    ASSERT_FALSE(simple_ob.DoesTradeExist(mock_bids_[0].GetTradeIdAsString()));
    ASSERT_EQ(simple_ob.GetVolumeAtPrice(mock_bids_[0].GetPrice(), mock_bids_[0].GetQuotationType()), 0);
}