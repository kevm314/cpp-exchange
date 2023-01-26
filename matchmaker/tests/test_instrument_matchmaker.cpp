#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "instrument_matchmaker.h"
#include "instrument_symbol.h"
#include "simple_order_book.h"
#include "simple_order_book_factory.h"
#include "tests/mock_simple_order_book.h"
#include "tests/mock_simple_order_book_factory.h"
#include "trade_event.h"
#include "trade_order.h"

class TestInstrumentMatchmaker : virtual public ::testing::Test {
    protected:
        // void SetUp() override {}
        // void TearDown() override {}
        uint32_t instrument_symbol_id = 1;
        matchmaker::InstrumentType instrument_type = matchmaker::InstrumentType::FUTURES_CONTRACT;
        int32_t base_currency = 100;
        int32_t quote_currency = 10;
        int32_t base_multiplier_k = 2;
        int32_t quote_multiplier_k = 3;
        int32_t maker_fee = 1;
        matchmaker::InstrumentSymbol futures_symbol = matchmaker::InstrumentSymbol(
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

class TestInstrumentMatchmakerBids : public TestInstrumentMatchmaker, public MockBidsOb {
    protected:
        void SetUp() override {
            MockBidsOb::SetUp();
        }
};

TEST_F(TestInstrumentMatchmaker, AddSymbol) {
    
    matchmaker::InstrumentMatchmaker instrument_matcmaker_futures = matchmaker::InstrumentMatchmaker(
        matchmaker::InstrumentType::FUTURES_CONTRACT,
        std::move(std::make_unique<matchmaker::SimpleOrderBookFactory>()));
    matchmaker::InstrumentMatchmaker instrument_matcmaker_shares = matchmaker::InstrumentMatchmaker(
        matchmaker::InstrumentType::SHARE,
        std::move(std::make_unique<matchmaker::SimpleOrderBookFactory>()));
    // test incorrect instrument type
    ASSERT_FALSE(instrument_matcmaker_shares.AddSymbol(&futures_symbol));
    // test add
    ASSERT_TRUE(instrument_matcmaker_futures.AddSymbol(&futures_symbol));
    ASSERT_EQ(instrument_matcmaker_futures.GetInstrumentSymbol(futures_symbol.GetInstrumentSymbolId()), &futures_symbol);
    ASSERT_EQ(instrument_matcmaker_futures.GetNumOrderBooks(), 1);
    // test double add
    ASSERT_FALSE(instrument_matcmaker_futures.AddSymbol(&futures_symbol));
}

TEST_F(TestInstrumentMatchmakerBids, ConsumeOrder) {
    
    // mocks
    std::shared_ptr<matchmaker::SimpleOrderBookFactory> mock_simple_ob_factory = std::make_shared<MockSimpleOrderBookFactory>();
    std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events = std::make_shared<std::vector<matchmaker::TradeEvent>>();
    matchmaker::InstrumentMatchmaker instrument_matchmaker_futures = matchmaker::InstrumentMatchmaker(
        matchmaker::InstrumentType::FUTURES_CONTRACT,
        mock_simple_ob_factory);
    
    EXPECT_CALL(*std::static_pointer_cast<MockSimpleOrderBookFactory>(mock_simple_ob_factory), FactoryProduce(&futures_symbol))
      .Times(1)
      .WillOnce(::testing::Return(std::make_unique<MockSimpleOrderBook>()));
    
    // test add
    ASSERT_TRUE(instrument_matchmaker_futures.AddSymbol(&futures_symbol));
    // test consume order
    ASSERT_EQ(instrument_matchmaker_futures.ConsumeOrder(mock_bids_[0], trade_events), OrderOutcomeType::ORDER_CANCEL_SUCCESS);
}