#include "json/exception.hpp"
#include "json/parser.hpp"

#include <gtest/gtest.h>

class parser : public ::testing::Test {
protected:
    json::value parse(const std::string &input) { return json::parse(input); }
};

// primitives
TEST_F(parser, parses_null) {
    auto val = json::parse("null");
    EXPECT_TRUE(val.is_null());
}

TEST_F(parser, parses_booleans) {
    auto t = json::parse("true");
    auto f = json::parse("false");
    EXPECT_TRUE(t.as_boolean());
    EXPECT_FALSE(f.as_boolean());
}

TEST_F(parser, parses_numbers) {
    auto zero = json::parse("0");
    auto positive = json::parse("42");
    auto negative = json::parse("-42");
    auto fraction = json::parse("3.14");
    auto negative_fraction = json::parse("-3.14");

    EXPECT_EQ(zero.as_number(), 0);
    EXPECT_EQ(positive.as_number(), 42);
    EXPECT_EQ(negative.as_number(), -42);
    EXPECT_EQ(fraction.as_number(), 3.14);
    EXPECT_EQ(negative_fraction.as_number(), -3.14);
}

TEST_F(parser, parses_strings) {
    auto empty = json::parse("\"\"");
    auto simple = json::parse("\"hello\"");
    auto with_spaces = json::parse("\"hello world\"");
    auto with_numbers = json::parse("\"hello 123\"");
    auto with_symbols = json::parse("\"hello, world!\"");

    EXPECT_EQ(empty.as_string(), "");
    EXPECT_EQ(simple.as_string(), "hello");
    EXPECT_EQ(with_spaces.as_string(), "hello world");
    EXPECT_EQ(with_numbers.as_string(), "hello 123");
    EXPECT_EQ(with_symbols.as_string(), "hello, world!");
}

// arrays
TEST_F(parser, parses_arrays) {
    // empty
    auto empty = json::parse("[]");
    EXPECT_TRUE(empty.is_array());
    EXPECT_TRUE(empty.as_array().empty());

    // single
    auto null_arr = json::parse("[null]");
    EXPECT_EQ(null_arr.as_array().size(), 1);
    EXPECT_TRUE(null_arr[0].is_null());

    auto bool_arr = json::parse("[true]");
    EXPECT_TRUE(bool_arr[0].as_boolean());

    auto num_arr = json::parse("[42]");
    EXPECT_EQ(num_arr[0].as_number(), 42);

    auto str_arr = json::parse("[\"text\"]");
    EXPECT_EQ(str_arr[0].as_string(), "text");

    // multi
    auto mixed = json::parse("[null,true,42,\"text\"]");
    EXPECT_EQ(mixed.as_array().size(), 4);
    EXPECT_TRUE(mixed[0].is_null());
    EXPECT_TRUE(mixed[1].as_boolean());
    EXPECT_EQ(mixed[2].as_number(), 42);
    EXPECT_EQ(mixed[3].as_string(), "text");

    // whitespace
    auto spaced = json::parse("[ 1 , 2 , 3 ]");
    EXPECT_EQ(spaced.as_array().size(), 3);
    EXPECT_EQ(spaced[0].as_number(), 1);
    EXPECT_EQ(spaced[1].as_number(), 2);
    EXPECT_EQ(spaced[2].as_number(), 3);
}

// objects
TEST_F(parser, parses_objects) {
    // empty
    auto empty = json::parse("{}");
    EXPECT_TRUE(empty.is_object());
    EXPECT_TRUE(empty.as_object().empty());

    // single
    auto with_null = json::parse("{\"key\":null}");
    EXPECT_TRUE(with_null["key"].is_null());

    auto with_bool = json::parse("{\"key\":true}");
    EXPECT_TRUE(with_bool["key"].as_boolean());

    auto with_number = json::parse("{\"key\":42}");
    EXPECT_EQ(with_number["key"].as_number(), 42);

    auto with_string = json::parse("{\"key\":\"value\"}");
    EXPECT_EQ(with_string["key"].as_string(), "value");

    // multi
    auto multiple = json::parse("{\"a\":1,\"b\":true,\"c\":\"text\"}");
    EXPECT_EQ(multiple["a"].as_number(), 1);
    EXPECT_TRUE(multiple["b"].as_boolean());
    EXPECT_EQ(multiple["c"].as_string(), "text");

    // whitespace
    auto spaced = json::parse("{ \"a\" : 1 , \"b\" : 2 }");
    EXPECT_EQ(spaced["a"].as_number(), 1);
    EXPECT_EQ(spaced["b"].as_number(), 2);
}

// nesting
TEST_F(parser, parses_nested_structures) {
    // array in array
    auto nested_array = json::parse("[[1,2],[3,4]]");
    EXPECT_EQ(nested_array[0][0].as_number(), 1);
    EXPECT_EQ(nested_array[1][1].as_number(), 4);

    // object in array
    auto obj_in_array = json::parse("[{\"a\":1}]");
    EXPECT_EQ(obj_in_array[0]["a"].as_number(), 1);

    // array in object
    auto array_in_obj = json::parse("{\"arr\":[1,2]}");
    EXPECT_EQ(array_in_obj["arr"][0].as_number(), 1);

    // object in object
    auto nested_obj = json::parse("{\"obj\":{\"a\":1}}");
    EXPECT_EQ(nested_obj["obj"]["a"].as_number(), 1);
}

// combination of 'atoms'
TEST_F(parser, parses_complex_structure) {
    auto complex = json::parse(R"({
        "empty_array": [],
        "empty_object": {},
        "numbers": [1, -2, 3.14],
        "nested": {
            "array": [[1, 2], [3, 4]],
            "object": {"a": {"b": 3}}
        },
        "mixed": [
            {"k": "v"},
            [1, 2],
            {"a": [{"b": 1}]}
        ]
    })");

    EXPECT_TRUE(complex["empty_array"].as_array().empty());
    EXPECT_TRUE(complex["empty_object"].as_object().empty());
    EXPECT_EQ(complex["numbers"][2].as_number(), 3.14);
    EXPECT_EQ(complex["nested"]["array"][1][1].as_number(), 4);
    EXPECT_EQ(complex["mixed"][2]["a"][0]["b"].as_number(), 1);
}

// common error cases
TEST_F(parser, detects_common_invalid_syntax) {
    EXPECT_THROW(json::parse(""), json::parse_error);          // empty input
    EXPECT_THROW(json::parse("    "), json::parse_error);      // only whitespace
    EXPECT_THROW(json::parse("["), json::parse_error);         // unclosed array
    EXPECT_THROW(json::parse("]"), json::parse_error);         // unexpected close
    EXPECT_THROW(json::parse("{"), json::parse_error);         // unclosed object
    EXPECT_THROW(json::parse("}"), json::parse_error);         // unexpected close
    EXPECT_THROW(json::parse("[,]"), json::parse_error);       // extra comma
    EXPECT_THROW(json::parse("[1,]"), json::parse_error);      // trailing comma
    EXPECT_THROW(json::parse("[1 2]"), json::parse_error);     // missing comma
    EXPECT_THROW(json::parse("{a:1}"), json::parse_error);     // missing quotes
    EXPECT_THROW(json::parse("{\"a\" 1}"), json::parse_error); // missing colon
    EXPECT_THROW(json::parse("1 2"), json::parse_error);       // multiple values
}

TEST_F(parser, detects_common_invalid_values) {
    EXPECT_THROW(json::parse("01"), json::parse_error);        // leading zero
    EXPECT_THROW(json::parse("1."), json::parse_error);        // trailing dot
    EXPECT_THROW(json::parse(".5"), json::parse_error);        // leading dot
    EXPECT_THROW(json::parse("TRUE"), json::parse_error);      // wrong case
    EXPECT_THROW(json::parse("NULL"), json::parse_error);      // wrong case
    EXPECT_THROW(json::parse("undefined"), json::parse_error); // invalid literal
}
