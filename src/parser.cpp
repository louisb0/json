#include "json/parser.hpp"
#include "profiler/profiler.hpp"
#include "json/exception.hpp"

#include <cassert>
#include <fstream>
#include <unordered_map>

namespace json {

class parser {
private:
    std::string m_src;
    size_t m_pos;
    size_t m_line;

public:
    parser(std::string json) : m_src(json), m_pos(0), m_line(1) {}

    [[nodiscard]] value parse() {
        skip_whitespace();
        if (is_at_end()) {
            throw parse_error("Empty input.", m_line);
        }

        value result = parse_value();

        skip_whitespace();
        if (!is_at_end()) {
            throw parse_error("Unexpected characters after JSON value.", m_line);
        }

        return result;
    }

private:
    value parse_value() {
        PROFILE_FUNCTION();

        skip_whitespace();
        if (is_at_end()) {
            throw parse_error("Unexpected end of input.", m_line);
        }

        char c = peek();
        switch (c) {
        case 'n':
            consume("null", "Expected 'null'.");
            return value();
        case 't':
        case 'f':
            return value(parse_boolean());
        case '"':
            return value(parse_string());
        case '[':
            return value(parse_array());
        case '{':
            return value(parse_object());
        default:
            if (c == '-' || std::isdigit(c)) {
                return value(parse_number());
            }

            throw parse_error("Unexpected character '" + std::to_string(c) + "' in JSON input.",
                              m_line);
        }
    }

    bool parse_boolean() {
        if (peek() == 't') {
            consume("true", "Expected 'true'.");
            return true;
        } else if (peek() == 'f') {
            consume("false", "Expected 'false'.");
            return false;
        }

        throw parse_error("Expected boolean value.", m_line);
    }

    std::string parse_string() {
        consume('"', "Expected start of string.");

        std::string result;
        while (!is_at_end() && peek() != '"') {
            result += advance();
        }

        consume('"', "Unterminated string literal.");
        return result;
    }

    std::vector<value> parse_array() {
        consume('[', "Expected start of array.");

        auto arr = std::vector<value>();

        skip_whitespace();
        if (match(']')) {
            return arr;
        }

        while (true) {
            arr.push_back(parse_value());

            skip_whitespace();
            if (match(']')) {
                break;
            }

            consume(',', "Expected ',' between array members");
            skip_whitespace();
        }

        return arr;
    }

    std::unordered_map<std::string, value> parse_object() {
        consume('{', "Expected start of object.");

        auto obj = std::unordered_map<std::string, value>();

        skip_whitespace();
        if (match('}')) {
            return obj;
        }

        while (true) {
            std::string key = parse_string();

            skip_whitespace();
            consume(':', "Expected ':' after object key.");

            value val = parse_value();
            obj[key] = std::move(val);

            skip_whitespace();
            if (match('}')) {
                break;
            }

            consume(',', "Expected ',' between object members");
            skip_whitespace();
        }

        return obj;
    }

    double parse_number() {
        assert(peek() == '-' || std::isdigit(peek()));
        std::string result;

        if (match('-')) {
            result += '-';
        }

        if (peek() == '0') {
            result += advance();

            if (std::isdigit(peek())) {
                throw parse_error("Leading zeros not allowed.", m_line);
            }
        } else if (std::isdigit(peek())) {
            result += advance();

            while (std::isdigit(peek())) {
                result += advance();
            }
        } else {
            throw parse_error("Invalid number format.", m_line);
        }

        if (match('.')) {
            result += '.';

            if (!std::isdigit(peek())) {
                throw parse_error("Expected digit after decimal point.", m_line);
            }

            while (std::isdigit(peek())) {
                result += advance();
            }
        }

        return std::stod(result);
    }

    void consume(char expected, const std::string &message) {
        if (is_at_end() || peek() != expected) {
            throw parse_error(message, m_line);
        }

        advance();
    }

    void consume(const std::string &expected, const std::string &message) {
        for (char c : expected) {
            if (is_at_end() || peek() != c) {
                throw parse_error(message, m_line);
            }

            advance();
        }
    }

    [[nodiscard]] bool match(char expected) {
        if (is_at_end() || peek() != expected) {
            return false;
        }

        advance();
        return true;
    }

    [[nodiscard]] char peek() { return is_at_end() ? '\0' : m_src[m_pos]; }

    char advance() {
        if (is_at_end()) {
            throw parse_error("Unexpected end of input.", m_line);
        }

        return m_src[m_pos++];
    }

    [[nodiscard]] bool is_at_end() const { return m_pos >= m_src.length(); }

    void skip_whitespace() {
        while (!is_at_end() && std::isspace(peek())) {
            if (peek() == '\n') {
                m_line++;
            }

            advance();
        }
    }
};

value parse(const std::string &json) { return parser(json).parse(); }

value parse_file(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw parse_error("Unable to open file at '" + path + "'.", 0);
    }

    std::string json;
    {
#if PROFILER
        file.seekg(0, std::ios::end);
        size_t file_size = file.tellg();
        file.seekg(0, std::ios::beg);
#endif

        PROFILE_BANDWIDTH("read file", file_size);
        json = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    return parse(json);
}

} // namespace json
