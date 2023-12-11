// CoordinateTest.cpp
#include <gtest/gtest.h>
#include "move.h"

TEST(CoordinateTest, Add) {
    Coordinate a = {1.0, 2.0};
    Coordinate b = {3.0, 4.0};
    Coordinate result = add(a, b);
    EXPECT_FLOAT_EQ(result.x, 4.0);
    EXPECT_FLOAT_EQ(result.y, 6.0);
}

TEST(CoordinateTest, Mul) {
    Coordinate a = {1.0, 2.0};
    float s = 2.0;
    Coordinate result = mul(s, a);
    EXPECT_FLOAT_EQ(result.x, 2.0);
    EXPECT_FLOAT_EQ(result.y, 4.0);
}

TEST(CoordinateTest, Move) {
    Coordinate position = {1.0, 2.0};
    Coordinate velocity = {2.0, 3.0};
    float delta = 0.5;
    Coordinate result = move(position, velocity, delta);
    EXPECT_FLOAT_EQ(result.x, 2.0);
    EXPECT_FLOAT_EQ(result.y, 3.5);
}