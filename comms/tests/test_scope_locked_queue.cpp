
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "scope_locked_queue.h"

class TestScopeLockedQueue : public ::testing::Test {
    protected:
        // void SetUp() override {}
        // void TearDown() override {}
};

TEST_F(TestScopeLockedQueue, EnqueueDequeue) {
    comms::ScopeLockedQueue queue = comms::ScopeLockedQueue<std::string>();
    ASSERT_EQ(queue.Size(), 0);
    std::string a;
    ASSERT_FALSE(queue.Dequeue(a));
    std::string b = "b";
    // enqueue and ensure move operation works correctly
    ASSERT_TRUE(queue.Enqueue(std::move(b)));
    ASSERT_EQ(b, "");
    ASSERT_EQ(queue.Size(), 1);
    // dequeue and ensure move operation works correctly
    ASSERT_TRUE(queue.Dequeue(a));
    ASSERT_EQ(a, "b");
    ASSERT_EQ(queue.Size(), 0);
}