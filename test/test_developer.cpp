#include <gtest/gtest.h>
#include "developer.h"

TEST(DeveloperTest, ConstructorAndGetters) {
    Developer dev("John Doe");
    EXPECT_EQ(dev.get_name(), "John Doe");
}

TEST(DeveloperTest, SetName) {
    Developer dev("Initial Name");
    dev.set_name("New Name");
    EXPECT_EQ(dev.get_name(), "New Name");
}

TEST(DeveloperTest, NullSafety) {
    Developer* null_dev = nullptr;
}