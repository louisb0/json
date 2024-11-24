#include <gtest/gtest.h>

#include "json/exception.hpp"
#include "json/value.hpp"

class array : public ::testing::Test {
protected:
    void SetUp() override {
        empty = json::array();
        numbers = json::array({1, 2, 3});
        mixed = json::array({true, "test", 42});
        nested = json::array({json::array({1, 2}), json::array({3, 4})});
    }

    json::value empty;
    json::value numbers;
    json::value mixed;
    json::value nested;
};

// construction
TEST_F(array, constructor_creates_array) {
    EXPECT_EQ(empty.type(), json::value::Type::Array);
    EXPECT_EQ(numbers.type(), json::value::Type::Array);
    EXPECT_EQ(mixed.type(), json::value::Type::Array);
    EXPECT_EQ(nested.type(), json::value::Type::Array);
}

TEST_F(array, constructor_preserves_values) {
    EXPECT_TRUE(empty.as_array().empty());
    EXPECT_EQ(numbers.as_array().size(), 3);
    EXPECT_EQ(mixed.as_array().size(), 3);
    EXPECT_EQ(nested.as_array().size(), 2);
}

// type checking
TEST_F(array, is_array_returns_true) {
    EXPECT_TRUE(empty.is_array());
    EXPECT_TRUE(numbers.is_array());
    EXPECT_TRUE(mixed.is_array());
    EXPECT_TRUE(nested.is_array());
}

TEST_F(array, other_type_checks_return_false) {
    EXPECT_FALSE(empty.is_null());
    EXPECT_FALSE(empty.is_boolean());
    EXPECT_FALSE(empty.is_number());
    EXPECT_FALSE(empty.is_string());
    EXPECT_FALSE(empty.is_object());
}

// access operations
TEST_F(array, as_array_returns_correct_values) {
    EXPECT_EQ(numbers.as_array()[0].as_number(), 1);
    EXPECT_EQ(numbers.as_array()[1].as_number(), 2);
    EXPECT_EQ(numbers.as_array()[2].as_number(), 3);

    EXPECT_TRUE(mixed.as_array()[0].as_boolean());
    EXPECT_EQ(mixed.as_array()[1].as_string(), "test");
    EXPECT_EQ(mixed.as_array()[2].as_number(), 42);
}

TEST_F(array, operator_index_access) {
    EXPECT_EQ(numbers[0].as_number(), 1);
    EXPECT_EQ(numbers[1].as_number(), 2);
    EXPECT_EQ(numbers[2].as_number(), 3);
}

TEST_F(array, operator_index_throws_on_invalid_index) {
    EXPECT_THROW(numbers[3], json::access_error);
    EXPECT_THROW(empty[0], json::access_error);
}

TEST_F(array, other_as_methods_throw_type_error) {
    EXPECT_THROW(empty.as_boolean(), json::type_error);
    EXPECT_THROW(empty.as_number(), json::type_error);
    EXPECT_THROW(empty.as_string(), json::type_error);
    EXPECT_THROW(empty.as_object(), json::type_error);
}

TEST_F(array, try_array_returns_correct_values) {
    auto ptr = numbers.try_array();
    EXPECT_TRUE(ptr);
    EXPECT_EQ((*ptr)[0].as_number(), 1);
}

TEST_F(array, other_try_methods_return_nullopt) {
    EXPECT_FALSE(empty.try_boolean());
    EXPECT_FALSE(empty.try_number());
    EXPECT_FALSE(empty.try_string());
    EXPECT_FALSE(empty.try_object());
}

// comparisons
TEST_F(array, equals_same_value) {
    EXPECT_EQ(empty, json::array());
    EXPECT_EQ(numbers, json::array({1, 2, 3}));
}

TEST_F(array, not_equals_different_value) {
    EXPECT_NE(empty, numbers);
    EXPECT_NE(numbers, json::array({1, 2}));
    EXPECT_NE(numbers, json::array({1, 2, 4}));
}

TEST_F(array, not_equals_other_types) {
    EXPECT_NE(empty, json::null());
    EXPECT_NE(empty, json::value(true));
    EXPECT_NE(empty, json::value(42));
    EXPECT_NE(empty, json::value("[]"));
    EXPECT_NE(empty, json::object({}));
}
