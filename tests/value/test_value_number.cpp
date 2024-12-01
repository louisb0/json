#include <gtest/gtest.h>

#include "json/exception.hpp"
#include "json/value.hpp"

class number : public ::testing::Test {
protected:
    void SetUp() override {
        int_value = json::value(42);
        double_value = json::value(3.14);
        zero = json::value(0);
        negative = json::value(-1);
    }

    json::value int_value;
    json::value double_value;
    json::value zero;
    json::value negative;
};

// construction
TEST_F(number, int_constructor_creates_number) {
    EXPECT_EQ(int_value.type(), json::value_t::Number);
}

TEST_F(number, double_constructor_creates_number) {
    EXPECT_EQ(double_value.type(), json::value_t::Number);
}

TEST_F(number, constructor_preserves_value) {
    EXPECT_EQ(int_value.as_number(), 42.0);
    EXPECT_EQ(double_value.as_number(), 3.14);
    EXPECT_EQ(zero.as_number(), 0.0);
    EXPECT_EQ(negative.as_number(), -1.0);
}

// type checking
TEST_F(number, is_number_returns_true) {
    EXPECT_TRUE(int_value.is_number());
    EXPECT_TRUE(double_value.is_number());
    EXPECT_TRUE(zero.is_number());
    EXPECT_TRUE(negative.is_number());
}

TEST_F(number, other_type_checks_return_false) {
    EXPECT_FALSE(int_value.is_null());
    EXPECT_FALSE(int_value.is_boolean());
    EXPECT_FALSE(int_value.is_string());
    EXPECT_FALSE(int_value.is_array());
    EXPECT_FALSE(int_value.is_object());
}

// access operations
TEST_F(number, as_number_returns_correct_value) {
    EXPECT_DOUBLE_EQ(int_value.as_number(), 42.0);
    EXPECT_DOUBLE_EQ(double_value.as_number(), 3.14);
}

TEST_F(number, other_as_methods_throw_type_error) {
    EXPECT_THROW(int_value.as_boolean(), json::type_error);
    EXPECT_THROW(int_value.as_string(), json::type_error);
    EXPECT_THROW(int_value.as_array(), json::type_error);
    EXPECT_THROW(int_value.as_object(), json::type_error);
}

TEST_F(number, try_number_returns_correct_value) {
    auto int_opt = int_value.try_number();
    auto double_opt = double_value.try_number();

    EXPECT_TRUE(int_opt.has_value());
    EXPECT_TRUE(double_opt.has_value());
    EXPECT_DOUBLE_EQ(int_opt.value(), 42.0);
    EXPECT_DOUBLE_EQ(double_opt.value(), 3.14);
}

TEST_F(number, other_try_methods_return_nullopt) {
    EXPECT_FALSE(int_value.try_boolean());
    EXPECT_FALSE(int_value.try_string());
    EXPECT_FALSE(int_value.try_array());
    EXPECT_FALSE(int_value.try_object());
}

// comparisons
TEST_F(number, equals_same_value) {
    EXPECT_EQ(int_value, json::value(42));
    EXPECT_EQ(double_value, json::value(3.14));
    EXPECT_EQ(zero, json::value(0));
    EXPECT_EQ(negative, json::value(-1));
}

TEST_F(number, not_equals_different_value) {
    EXPECT_NE(int_value, double_value);
    EXPECT_NE(zero, negative);
}

TEST_F(number, not_equals_other_types) {
    EXPECT_NE(int_value, json::null());
    EXPECT_NE(int_value, json::boolean(true));
    EXPECT_NE(int_value, json::string("42"));
    EXPECT_NE(int_value, json::array());
    EXPECT_NE(int_value, json::object({}));
}

// display
TEST_F(number, string_and_streams) {
    std::stringstream ss;
    ss << int_value << " " << double_value << " " << negative;
    EXPECT_EQ(ss.str(), "42 3.14 -1");
}
