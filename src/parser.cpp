#include "parser.hpp"
#include "json_value.hpp"

#include <cassert>
#include <cctype>
#include <stdexcept>
#include <unordered_map>

#define UNREACHABLE() assert(!"Unreachable")

namespace json {

class Parser {
private:
    std::string m_src;
    std::size_t m_pos;

public:
    Parser(std::string src) : m_src(src) {}

    JSONValue parse() {
        skip_whitespace();
        if (is_at_end()) {
            throw std::runtime_error("Empty input.");
        }

        JSONValue result = value();

        skip_whitespace();
        if (m_pos < m_src.length()) {
            throw std::runtime_error("Unexpected characters after JSON value.");
        }

        return result;
    }

private:
    [[nodiscard]] JSONValue value() {
        skip_whitespace();

        if (is_at_end()) {
            throw std::runtime_error("Unexpected end of input.");
        }

        char c = peek();
        switch (c) {
        case '{':
            return JSONValue(object());
        case '[':
            return JSONValue(array());
        case '"':
            return JSONValue(new std::string(string()));
        case 't':
        case 'f':
            return JSONValue(boolean());
        case 'n':
            consume('n', "Expected 'null'.");
            consume('u', "Expected 'null'.");
            consume('l', "Expected 'null'.");
            consume('l', "Expected 'null'.");
            return JSONValue();
        default:
            if (c == '-' || std::isdigit(c)) {
                return JSONValue(number());
            }
            throw std::runtime_error("Unexpected character in JSON input");
        }
    }

    [[nodiscard]] std::unordered_map<std::string, JSONValue> *object() {
        consume('{', "Expected object to begin with '{'.");

        auto obj = new std::unordered_map<std::string, JSONValue>();

        skip_whitespace();
        if (match('}')) {
            return obj;
        }

        while (true) {
            skip_whitespace();
            std::string key = string();

            skip_whitespace();
            consume(':', "Expected ':' after object key.");

            JSONValue val = value();
            (*obj)[key] = std::move(val);

            skip_whitespace();
            if (match('}')) {
                break;
            }

            consume(',', "Expected ',' between object members");
        }

        return obj;
    }

    [[nodiscard]] std::vector<JSONValue> *array() {
        consume('[', "Expected array to start with '['.");

        auto arr = new std::vector<JSONValue>();

        skip_whitespace();
        if (match(']')) {
            return arr;
        }

        while (true) {
            arr->push_back(value());

            skip_whitespace();
            if (match(']')) {
                break;
            }

            consume(',', "Expected ',' between object members");
        }

        return arr;
    }

    // NOTE(louis): not spec compliant, e.g. escape characters
    [[nodiscard]] std::string string() {
        consume('"', "Expected string to begin with '\"'");

        std::string result;
        while (!is_at_end() && peek() != '"') {
            result += advance();
        }

        consume('"', "Unterminated string literal.");
        return result;
    }

    [[nodiscard]] bool boolean() {
        assert(peek() == 't' || peek() == 'f');

        if (match('t')) {
            consume('r', "Expected 'true'.");
            consume('u', "Expected 'true'.");
            consume('e', "Expected 'true'.");
            return true;
        } else if (match('f')) {
            consume('a', "Expected 'false'.");
            consume('l', "Expected 'false'.");
            consume('s', "Expected 'false'.");
            consume('e', "Expected 'false'.");
            return false;
        }

        UNREACHABLE();
    }

    // NOTE(louis): not spec compliant, missing things like exponents
    [[nodiscard]] int number() {
        assert(peek() == '-' || std::isdigit(peek()));

        std::string num;

        if (peek() == '-') {
            num += advance();
        }

        if (!std::isdigit(peek())) {
            throw std::runtime_error("Invalid number format.");
        }

        while (!is_at_end() && std::isdigit(peek())) {
            num += advance();
        }

        return std::stod(num);
    }

    // TODO(louis): an string-type 'expected' would be nicer to use
    void consume(char expected, const std::string &message) {
        if (is_at_end() || peek() != expected) {
            throw std::runtime_error(message);
        }

        advance();
    }

    [[nodiscard]] bool match(char expected) {
        if (is_at_end() || peek() != expected) {
            return false;
        }

        advance();
        return true;
    }

    [[nodiscard]] char peek() {
        if (is_at_end()) {
            throw std::runtime_error("Unexpected end of input.");
        }
        return m_src[m_pos];
    }

    char advance() {
        if (is_at_end()) {
            throw std::runtime_error("Unexpected end of input.");
        }

        return m_src[m_pos++];
    }

    bool is_at_end() const { return m_pos >= m_src.length(); }

    void skip_whitespace() {
        while (!is_at_end() && std::isspace(peek())) {
            advance();
        }
    }
};

[[nodiscard]] JSONValue parse(std::string src) { return Parser(std::move(src)).parse(); }

} // namespace json
