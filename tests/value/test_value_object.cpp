#include <gtest/gtest.h>

#include "json/exception.hpp"
#include "json/value.hpp"

class object : public ::testing::Test {
protected:
    void SetUp() override {
        empty = json::object({});
        simple = json::object({{"number", 42}, {"string", "test"}, {"bool", true}});
        nested = json::object(
            {{"array", json::array({1, 2, 3})}, {"object", json::object({{"nested", "value"}})}});
    }

    json::value empty;
    json::value simple;
    json::value nested;
};

// construction
TEST_F(object, constructor_creates_object) {
    EXPECT_EQ(empty.type(), json::value_t::Object);
    EXPECT_EQ(simple.type(), json::value_t::Object);
    EXPECT_EQ(nested.type(), json::value_t::Object);
}

TEST_F(object, constructor_preserves_values) {
    EXPECT_TRUE(empty.as_object().empty());
    EXPECT_EQ(simple.as_object().size(), 3);
    EXPECT_EQ(nested.as_object().size(), 2);
}

// type checking
TEST_F(object, is_object_returns_true) {
    EXPECT_TRUE(empty.is_object());
    EXPECT_TRUE(simple.is_object());
    EXPECT_TRUE(nested.is_object());
}

TEST_F(object, other_type_checks_return_false) {
    EXPECT_FALSE(empty.is_null());
    EXPECT_FALSE(empty.is_boolean());
    EXPECT_FALSE(empty.is_number());
    EXPECT_FALSE(empty.is_string());
    EXPECT_FALSE(empty.is_array());
}

// access operations
TEST_F(object, as_object_returns_correct_values) {
    auto &obj = simple.as_object();
    EXPECT_EQ(obj.at("number").as_number(), 42);
    EXPECT_EQ(obj.at("string").as_string(), "test");
    EXPECT_TRUE(obj.at("bool").as_boolean());
}

TEST_F(object, operator_index_access) {
    EXPECT_EQ(simple["number"].as_number(), 42);
    EXPECT_EQ(simple["string"].as_string(), "test");
    EXPECT_TRUE(simple["bool"].as_boolean());
}

TEST_F(object, operator_index_throws_on_invalid_key) {
    EXPECT_THROW(simple["nonexistent"], json::access_error);
    EXPECT_THROW(empty["key"], json::access_error);
}

TEST_F(object, other_as_methods_throw_type_error) {
    EXPECT_THROW(empty.as_boolean(), json::type_error);
    EXPECT_THROW(empty.as_number(), json::type_error);
    EXPECT_THROW(empty.as_string(), json::type_error);
    EXPECT_THROW(empty.as_array(), json::type_error);
}

TEST_F(object, try_object_returns_correct_values) {
    auto ptr = simple.try_object();
    EXPECT_TRUE(ptr);
    EXPECT_EQ(ptr->at("number").as_number(), 42);
}

TEST_F(object, other_try_methods_return_nullopt) {
    EXPECT_FALSE(empty.try_boolean());
    EXPECT_FALSE(empty.try_number());
    EXPECT_FALSE(empty.try_string());
    EXPECT_FALSE(empty.try_array());
}

// comparisons
TEST_F(object, equals_same_value) {
    EXPECT_EQ(empty, json::object({}));
    EXPECT_EQ(simple, json::object({{"number", 42}, {"string", "test"}, {"bool", true}}));
}

TEST_F(object, not_equals_different_value) {
    EXPECT_NE(empty, simple);
    EXPECT_NE(simple, json::object({{"number", 42}}));
    EXPECT_NE(simple, json::object({{"number", 42}, {"string", "different"}, {"bool", true}}));
}

TEST_F(object, not_equals_other_types) {
    EXPECT_NE(empty, json::null());
    EXPECT_NE(empty, json::value(true));
    EXPECT_NE(empty, json::value(42));
    EXPECT_NE(empty, json::value("{}"));
    EXPECT_NE(empty, json::array());
}

// nested access
TEST_F(object, nested_array_access) {
    EXPECT_EQ(nested["array"][0].as_number(), 1);
    EXPECT_EQ(nested["array"][1].as_number(), 2);
    EXPECT_EQ(nested["array"][2].as_number(), 3);
}

TEST_F(object, nested_object_access) { EXPECT_EQ(nested["object"]["nested"].as_string(), "value"); }
