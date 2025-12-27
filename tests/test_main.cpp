#include <gtest/gtest.h>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    // You can add global setup here (like initializing hardware counters)
    return RUN_ALL_TESTS();
}
