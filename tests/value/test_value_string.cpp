#include <gtest/gtest.h>

#include "json/exception.hpp"
#include "json/value.hpp"

class string : public ::testing::Test {
protected:
    void SetUp() override {
        empty = json::value("");
        simple = json::value("test");
        const_char = json::value("const char*");
        std_string = json::value(std::string("std::string"));
    }

    json::value empty;
    json::value simple;
    json::value const_char;
    json::value std_string;
};

// construction
TEST_F(string, constructor_creates_string) {
    EXPECT_EQ(empty.type(), json::value_t::String);
    EXPECT_EQ(simple.type(), json::value_t::String);
    EXPECT_EQ(const_char.type(), json::value_t::String);
    EXPECT_EQ(std_string.type(), json::value_t::String);
}

TEST_F(string, constructor_preserves_value) {
    EXPECT_EQ(empty.as_string(), "");
    EXPECT_EQ(simple.as_string(), "test");
    EXPECT_EQ(const_char.as_string(), "const char*");
    EXPECT_EQ(std_string.as_string(), "std::string");
}

// type checking
TEST_F(string, is_string_returns_true) {
    EXPECT_TRUE(empty.is_string());
    EXPECT_TRUE(simple.is_string());
    EXPECT_TRUE(const_char.is_string());
    EXPECT_TRUE(std_string.is_string());
}

TEST_F(string, other_type_checks_return_false) {
    EXPECT_FALSE(simple.is_null());
    EXPECT_FALSE(simple.is_boolean());
    EXPECT_FALSE(simple.is_number());
    EXPECT_FALSE(simple.is_array());
    EXPECT_FALSE(simple.is_object());
}

// access operations
TEST_F(string, as_string_returns_correct_value) {
    EXPECT_EQ(empty.as_string(), "");
    EXPECT_EQ(simple.as_string(), "test");
}

TEST_F(string, other_as_methods_throw_type_error) {
    EXPECT_THROW(simple.as_boolean(), json::type_error);
    EXPECT_THROW(simple.as_number(), json::type_error);
    EXPECT_THROW(simple.as_array(), json::type_error);
    EXPECT_THROW(simple.as_object(), json::type_error);
}

TEST_F(string, try_string_returns_correct_value) {
    auto empty_ptr = empty.try_string();
    auto simple_ptr = simple.try_string();

    EXPECT_TRUE(empty_ptr);
    EXPECT_TRUE(simple_ptr);
    EXPECT_EQ(*empty_ptr, "");
    EXPECT_EQ(*simple_ptr, "test");
}

TEST_F(string, other_try_methods_return_nullopt) {
    EXPECT_FALSE(simple.try_boolean());
    EXPECT_FALSE(simple.try_number());
    EXPECT_FALSE(simple.try_array());
    EXPECT_FALSE(simple.try_object());
}

// comparisons
TEST_F(string, equals_same_value) {
    EXPECT_EQ(empty, json::value(""));
    EXPECT_EQ(simple, json::value("test"));
    EXPECT_EQ(const_char, json::value("const char*"));
}

TEST_F(string, not_equals_different_value) {
    EXPECT_NE(empty, simple);
    EXPECT_NE(simple, const_char);
}

TEST_F(string, not_equals_other_types) {
    EXPECT_NE(simple, json::null());
    EXPECT_NE(simple, json::boolean(true));
    EXPECT_NE(simple, json::number(42));
    EXPECT_NE(simple, json::array());
    EXPECT_NE(simple, json::object({}));
}

TEST_F(string, handles_special_characters) {
    json::value special("Special\nChars\t\"Quote\"\\\r\n");
    EXPECT_EQ(special.as_string(), "Special\nChars\t\"Quote\"\\\r\n");
}

TEST_F(string, handles_empty_string) {
    EXPECT_TRUE(empty.is_string());
    EXPECT_EQ(empty.as_string().length(), 0);
}
