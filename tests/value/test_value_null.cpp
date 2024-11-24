#include <gtest/gtest.h>

#include "json/exception.hpp"
#include "json/value.hpp"

class null : public ::testing::Test {
protected:
    void SetUp() override {
        default_constructed = json::value();
        nullptr_constructed = json::value(nullptr);
        helper_constructed = json::null();
    }

    json::value default_constructed;
    json::value nullptr_constructed;
    json::value helper_constructed;
};

// construction
TEST_F(null, default_constructor_creates_null) {
    EXPECT_EQ(default_constructed.type(), json::value::Type::Null);
}

TEST_F(null, nullptr_constructor_creates_null) {
    EXPECT_EQ(nullptr_constructed.type(), json::value::Type::Null);
}

TEST_F(null, helper_function_creates_null) {
    EXPECT_EQ(helper_constructed.type(), json::value::Type::Null);
}

// type checking
TEST_F(null, is_null_returns_true) {
    EXPECT_TRUE(default_constructed.is_null());
    EXPECT_TRUE(nullptr_constructed.is_null());
    EXPECT_TRUE(helper_constructed.is_null());
}

TEST_F(null, other_type_checks_return_false) {
    EXPECT_FALSE(default_constructed.is_boolean());
    EXPECT_FALSE(default_constructed.is_number());
    EXPECT_FALSE(default_constructed.is_string());
    EXPECT_FALSE(default_constructed.is_array());
    EXPECT_FALSE(default_constructed.is_object());
}

// access operations
TEST_F(null, as_methods_throw_type_error) {
    EXPECT_THROW(default_constructed.as_boolean(), json::type_error);
    EXPECT_THROW(default_constructed.as_number(), json::type_error);
    EXPECT_THROW(default_constructed.as_string(), json::type_error);
    EXPECT_THROW(default_constructed.as_array(), json::type_error);
    EXPECT_THROW(default_constructed.as_object(), json::type_error);
}

TEST_F(null, try_methods_return_nullopt) {
    EXPECT_FALSE(default_constructed.try_boolean());
    EXPECT_FALSE(default_constructed.try_number());
    EXPECT_FALSE(default_constructed.try_string());
    EXPECT_FALSE(default_constructed.try_array());
    EXPECT_FALSE(default_constructed.try_object());
}

// comparisons
TEST_F(null, equals_null) {
    EXPECT_EQ(default_constructed, nullptr_constructed);
    EXPECT_EQ(nullptr_constructed, helper_constructed);
    EXPECT_EQ(helper_constructed, default_constructed);
}

TEST_F(null, not_equals_other_types) {
    EXPECT_NE(default_constructed, json::boolean(false));
    EXPECT_NE(default_constructed, json::number(0));
    EXPECT_NE(default_constructed, json::string(""));
    EXPECT_NE(default_constructed, json::array());
    EXPECT_NE(default_constructed, json::object({}));
}

// copy / move
TEST_F(null, copy_constructor_preserves_null) {
    json::value copy(default_constructed);
    EXPECT_TRUE(copy.is_null());
    EXPECT_EQ(copy, default_constructed);
}

TEST_F(null, copy_assignment_preserves_null) {
    json::value copy;
    copy = default_constructed;
    EXPECT_TRUE(copy.is_null());
    EXPECT_EQ(copy, default_constructed);
}

TEST_F(null, move_constructor_preserves_null) {
    json::value source = json::null();
    json::value moved(std::move(source));

    EXPECT_TRUE(moved.is_null());
    EXPECT_TRUE(source.is_null());
}

TEST_F(null, move_assignment_preserves_null) {
    json::value source = json::null();
    json::value moved;
    moved = std::move(source);
    EXPECT_TRUE(moved.is_null());
    EXPECT_TRUE(source.is_null());
}

TEST_F(null, self_assignment_is_handled) {
    json::value val = json::null();
    val = val;
    EXPECT_TRUE(val.is_null());
}

TEST_F(null, chained_assignment_works) {
    json::value a, b, c;
    a = b = c = json::null();
    EXPECT_TRUE(a.is_null());
    EXPECT_TRUE(b.is_null());
    EXPECT_TRUE(c.is_null());
}
