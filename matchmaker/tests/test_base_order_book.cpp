#include <array>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "order_outcome_types.h"
#include "order_request_types.h"
#include "trade_order.h"
#include "trade_order_types.h"
#include "trade_quotation_types.h"

class TestBaseOrderBook : public ::testing::Test {
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

TEST_F(TestBaseOrderBook, SimpleConstruction) {
    matchmaker::BaseOrderBook base_ob = matchmaker::BaseOrderBook(eur_usd_pair);
    ASSERT_EQ(base_ob.GetInstrumentSymbol().GetInstrumentSymbolId(), eur_usd_pair.GetInstrumentSymbolId());
}