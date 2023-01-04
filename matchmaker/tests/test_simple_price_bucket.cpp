#include <array>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "simple_price_bucket.h"

class TestSimplePriceBucket : public ::testing::Test {
    protected:
        // void SetUp() override {}
        // void TearDown() override {}
        uint32_t instrument_symbol_id = 1;
};

TEST_F(TestSimplePriceBucket, SimpleConstruction) {
    uint64_t price = 10;
    matchmaker::SimplePriceBucket simple_pb = matchmaker::SimplePriceBucket(price);
    ASSERT_EQ(simple_pb.GetPrice(), price);
    ASSERT_EQ(simple_pb.GetTotalVolume(), 0);
    ASSERT_EQ(simple_pb.GetFirstOrder(), nullptr);
}