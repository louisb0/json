#include <gtest/gtest.h>

#include "json/exception.hpp"
#include "json/value.hpp"

class boolean : public ::testing::Test {
protected:
    void SetUp() override {
        true_value = json::value(true);
        false_value = json::value(false);
        bool_constructed = json::boolean(true);
    }

    json::value true_value;
    json::value false_value;
    json::value bool_constructed;
};

// construction
TEST_F(boolean, constructor_creates_boolean) {
    EXPECT_EQ(true_value.type(), json::value_t::Boolean);
    EXPECT_EQ(false_value.type(), json::value_t::Boolean);
    EXPECT_EQ(bool_constructed.type(), json::value_t::Boolean);
}

TEST_F(boolean, constructor_preserves_value) {
    EXPECT_EQ(true_value.as_boolean(), true);
    EXPECT_EQ(false_value.as_boolean(), false);
    EXPECT_EQ(bool_constructed.as_boolean(), true);
}

// type checking
TEST_F(boolean, is_boolean_returns_true) {
    EXPECT_TRUE(true_value.is_boolean());
    EXPECT_TRUE(false_value.is_boolean());
    EXPECT_TRUE(bool_constructed.is_boolean());
}

TEST_F(boolean, other_type_checks_return_false) {
    EXPECT_FALSE(true_value.is_null());
    EXPECT_FALSE(true_value.is_number());
    EXPECT_FALSE(true_value.is_string());
    EXPECT_FALSE(true_value.is_array());
    EXPECT_FALSE(true_value.is_object());
}

// access operations
TEST_F(boolean, as_boolean_returns_correct_value) {
    EXPECT_TRUE(true_value.as_boolean());
    EXPECT_FALSE(false_value.as_boolean());
}

TEST_F(boolean, other_as_methods_throw_type_error) {
    EXPECT_THROW(true_value.as_number(), json::type_error);
    EXPECT_THROW(true_value.as_string(), json::type_error);
    EXPECT_THROW(true_value.as_array(), json::type_error);
    EXPECT_THROW(true_value.as_object(), json::type_error);
}

TEST_F(boolean, try_boolean_returns_correct_value) {
    auto true_opt = true_value.try_boolean();
    auto false_opt = false_value.try_boolean();

    EXPECT_TRUE(true_opt.has_value());
    EXPECT_TRUE(false_opt.has_value());
    EXPECT_TRUE(true_opt.value());
    EXPECT_FALSE(false_opt.value());
}

TEST_F(boolean, other_try_methods_return_nullopt) {
    EXPECT_FALSE(true_value.try_number());
    EXPECT_FALSE(true_value.try_string());
    EXPECT_FALSE(true_value.try_array());
    EXPECT_FALSE(true_value.try_object());
}

// comparisons
TEST_F(boolean, equals_same_value) {
    EXPECT_EQ(true_value, json::value(true));
    EXPECT_EQ(false_value, json::value(false));
    EXPECT_EQ(bool_constructed, json::value(true));
}

TEST_F(boolean, not_equals_different_value) {
    EXPECT_NE(true_value, false_value);
    EXPECT_NE(false_value, true_value);
}

TEST_F(boolean, not_equals_other_types) {
    EXPECT_NE(true_value, json::null());
    EXPECT_NE(true_value, json::number(0));
    EXPECT_NE(true_value, json::string("true"));
    EXPECT_NE(true_value, json::array());
    EXPECT_NE(true_value, json::object({}));
}

// display
TEST_F(boolean, string_and_streams) {
    std::stringstream ss;
    ss << true_value << false_value;
    EXPECT_EQ(ss.str(), "truefalse");
}
