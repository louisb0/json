#include <gtest/gtest.h>

#include "json/value.hpp"

class value_copy_move : public ::testing::Test {
protected:
    void SetUp() override {
        null_value = json::null();
        bool_value = json::value(true);
        number_value = json::value(42.5);
        string_value = json::value("test");
        array_value = json::array({1, 2, 3});
        object_value = json::object({{"key", "value"}});

        nested_value = json::object({{"array", json::array({1, "string", true})},
                                     {"object", json::object({{"nested", "value"}})}});
    }

    json::value null_value;
    json::value bool_value;
    json::value number_value;
    json::value string_value;
    json::value array_value;
    json::value object_value;
    json::value nested_value;
};

TEST_F(value_copy_move, copy_constructor_preserves_values) {
    EXPECT_EQ(json::value(null_value), null_value);
    EXPECT_EQ(json::value(bool_value), bool_value);
    EXPECT_EQ(json::value(number_value), number_value);
    EXPECT_EQ(json::value(string_value), string_value);
    EXPECT_EQ(json::value(array_value), array_value);
    EXPECT_EQ(json::value(object_value), object_value);
    EXPECT_EQ(json::value(nested_value), nested_value);
}

TEST_F(value_copy_move, copy_assignment_preserves_values) {
    json::value copy;

    copy = null_value;
    EXPECT_EQ(copy, null_value);

    copy = bool_value;
    EXPECT_EQ(copy, bool_value);

    copy = number_value;
    EXPECT_EQ(copy, number_value);

    copy = string_value;
    EXPECT_EQ(copy, string_value);

    copy = array_value;
    EXPECT_EQ(copy, array_value);

    copy = object_value;
    EXPECT_EQ(copy, object_value);

    copy = nested_value;
    EXPECT_EQ(copy, nested_value);
}

TEST_F(value_copy_move, move_constructor_transfers_ownership) {
    {
        json::value source = bool_value;
        json::value moved(std::move(source));
        EXPECT_EQ(moved, bool_value);
        EXPECT_TRUE(source.is_null());
    }

    {
        json::value source = nested_value;
        json::value moved(std::move(source));
        EXPECT_EQ(moved, nested_value);
        EXPECT_TRUE(source.is_null());
    }
}

TEST_F(value_copy_move, move_assignment_transfers_ownership) {
    json::value moved;

    {
        json::value source = bool_value;
        moved = std::move(source);
        EXPECT_EQ(moved, bool_value);
        EXPECT_TRUE(source.is_null());
    }

    {
        json::value source = nested_value;
        moved = std::move(source);
        EXPECT_EQ(moved, nested_value);
        EXPECT_TRUE(source.is_null());
    }
}

TEST_F(value_copy_move, self_operations_are_handled) {
    json::value val = nested_value;
    val = val;
    EXPECT_EQ(val, nested_value);

    val = std::move(val);
    EXPECT_TRUE(val.type() == json::value::Type::Object);
}

TEST_F(value_copy_move, chained_assignment_works) {
    json::value a, b, c;

    a = b = c = bool_value;
    EXPECT_EQ(a, bool_value);
    EXPECT_EQ(b, bool_value);
    EXPECT_EQ(c, bool_value);

    a = b = c = nested_value;
    EXPECT_EQ(a, nested_value);
    EXPECT_EQ(b, nested_value);
    EXPECT_EQ(c, nested_value);
}
