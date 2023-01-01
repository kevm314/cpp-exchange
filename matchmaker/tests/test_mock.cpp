#include <iostream>

#include "gtest/gtest.h"


TEST(Mock, Sample) {
    std::cout << "Test Hello world!" << std::endl;
    ASSERT_EQ(1, 0);
}