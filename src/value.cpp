#include "json/value.hpp"
#include "json/exception.hpp"

#include <cstddef>
#include <initializer_list>
#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>

namespace json {

struct value::storage {
    value_t type;
    std::variant<std::nullptr_t, bool, double, std::string, std::vector<value>,
                 std::unordered_map<std::string, value>>
        data;

    storage() : type(value_t::Null), data(nullptr) {}

    template <typename T> static storage create(value_t t, T &&value) {
        storage s;
        s.type = t;
        s.data = std::forward<T>(value);
        return s;
    }
};

value::value() : m_storage(std::make_unique<storage>()) {}

value::value(std::nullptr_t) : value() {}

value::value(bool value)
    : m_storage(std::make_unique<storage>(storage::create(value_t::Boolean, value))) {}

value::value(int value)
    : m_storage(
          std::make_unique<storage>(storage::create(value_t::Number, static_cast<double>(value)))) {}
value::value(double value)
    : m_storage(std::make_unique<storage>(storage::create(value_t::Number, value))) {}

value::value(const char *value)
    : m_storage(std::make_unique<storage>(storage::create(value_t::String, std::string(value)))) {}

value::value(std::string value)
    : m_storage(std::make_unique<storage>(storage::create(value_t::String, std::move(value)))) {}

value::value(std::vector<value> values)
    : m_storage(std::make_unique<storage>(storage::create(value_t::Array, std::move(values)))) {}

value::value(std::unordered_map<std::string, value> values)
    : m_storage(std::make_unique<storage>(storage::create(value_t::Object, std::move(values)))) {}

// TODO(louis): we can only have one of array/object due to constructor ambiuguity without a bunch
// more work. does it make sense for it to be object and not array? should we have either both
// or none?
value::value(std::initializer_list<std::pair<std::string, value>> init)
    : m_storage(std::make_unique<storage>(storage::create(
          value_t::Object, std::unordered_map<std::string, value>(init.begin(), init.end())))) {}

value::value(const value &other) : m_storage(std::make_unique<storage>(*other.m_storage)) {}

value &value::operator=(const value &other) {
    if (this != &other) {
        *m_storage = *other.m_storage;
    }
    return *this;
}

value::value(value &&other) noexcept : m_storage(std::move(other.m_storage)) {
    other.m_storage = std::make_unique<storage>();
}

value &value::operator=(value &&other) noexcept {
    if (this != &other) {
        m_storage = std::move(other.m_storage);
        other.m_storage = std::make_unique<storage>();
    }
    return *this;
};

value::~value() = default;

value value::array() { return value(std::vector<value>{}); }

value value::array(std::vector<value> values) { return value(std::move(values)); }

value_t value::type() const noexcept { return m_storage->type; }

bool value::is_null() const noexcept { return type() == value_t::Null; }
bool value::is_boolean() const noexcept { return type() == value_t::Boolean; }
bool value::is_number() const noexcept { return type() == value_t::Number; }
bool value::is_string() const noexcept { return type() == value_t::String; }
bool value::is_array() const noexcept { return type() == value_t::Array; }
bool value::is_object() const noexcept { return type() == value_t::Object; }

std::string value::stringify() const noexcept {
    switch (type()) {
    case value_t::Null:
        return "null";
    case value_t::Boolean:
        return as_boolean() ? "true" : "false";
    case value_t::Number: {
        std::stringstream ss;
        double num = as_number();

        ss << std::fixed << std::setprecision(10) << num;
        std::string str = ss.str();

        str.erase(str.find_last_not_of('0') + 1);
        if (str.back() == '.') {
            str.pop_back();
        }

        return str;
    }
    case value_t::String:
        return "\"" + as_string() + "\"";
    case value_t::Array: {
        const auto &arr = as_array();
        std::string res = "[";
        for (std::size_t i = 0; i < arr.size(); i++) {
            if (i > 0)
                res += ", ";
            res += arr[i].stringify();
        }
        res += "]";
        return res;
    }
    case value_t::Object: {
        const auto &obj = as_object();
        std::string res = "{";
        bool first = true;
        for (const auto &[key, val] : obj) {
            if (!first)
                res += ", ";

            res += "\"" + key + "\": " + val.stringify();
            first = false;
        }
        res += "}";
        return res;
    }
    default:
        return "???"; // preferably some kind of unreachable macro to blow everything up
    }
}

[[nodiscard]] static std::string type_name(value_t t) {
    switch (t) {
    case value_t::Null:
        return "null";
    case value_t::Boolean:
        return "boolean";
    case value_t::Number:
        return "number";
    case value_t::String:
        return "string";
    case value_t::Array:
        return "array";
    case value_t::Object:
        return "object";
    }
    return "unknown";
}

bool value::as_boolean() const {
    if (!is_boolean()) {
        throw type_error(type_name(value_t::Boolean), type_name(type()));
    }

    return std::get<bool>(m_storage->data);
}

double value::as_number() const {
    if (!is_number()) {
        throw type_error(type_name(value_t::Number), type_name(type()));
    }

    return std::get<double>(m_storage->data);
}

const std::string &value::as_string() const {
    if (!is_string()) {
        throw type_error(type_name(value_t::String), type_name(type()));
    }

    return std::get<std::string>(m_storage->data);
}

const std::vector<value> &value::as_array() const {
    if (!is_array()) {
        throw type_error(type_name(value_t::Array), type_name(type()));
    }

    return std::get<std::vector<value>>(m_storage->data);
}

const std::unordered_map<std::string, value> &value::as_object() const {
    if (!is_object()) {
        throw type_error(type_name(value_t::Object), type_name(type()));
    }

    return std::get<std::unordered_map<std::string, value>>(m_storage->data);
}

std::string &value::as_string() {
    if (!is_string()) {
        throw type_error(type_name(value_t::String), type_name(type()));
    }

    return std::get<std::string>(m_storage->data);
}

std::vector<value> &value::as_array() {
    if (!is_array()) {
        throw type_error(type_name(value_t::Array), type_name(type()));
    }

    return std::get<std::vector<value>>(m_storage->data);
}

std::unordered_map<std::string, value> &value::as_object() {
    if (!is_object()) {
        throw type_error(type_name(value_t::Object), type_name(type()));
    }

    return std::get<std::unordered_map<std::string, value>>(m_storage->data);
}

std::optional<bool> value::try_boolean() const noexcept {
    return is_boolean() ? std::make_optional(std::get<bool>(m_storage->data)) : std::nullopt;
}

std::optional<double> value::try_number() const noexcept {
    return is_number() ? std::make_optional(std::get<double>(m_storage->data)) : std::nullopt;
}

non_owning<const std::string> value::try_string() const noexcept {
    return is_string() ? &std::get<std::string>(m_storage->data) : nullptr;
}

non_owning<const std::vector<value>> value::try_array() const noexcept {
    return is_array() ? &std::get<std::vector<value>>(m_storage->data) : nullptr;
}

non_owning<const std::unordered_map<std::string, value>> value::try_object() const noexcept {
    return is_object() ? &std::get<std::unordered_map<std::string, value>>(m_storage->data)
                       : nullptr;
}

value &value::operator[](size_t index) {
    auto &arr = as_array();
    if (index >= arr.size()) {
        throw access_error("Array index out of bounds.");
    }

    return arr[index];
}

const value &value::operator[](size_t index) const {
    auto &arr = as_array();
    if (index >= arr.size()) {
        throw access_error("Array index out of bounds.");
    }

    return arr[index];
}

value &value::operator[](const std::string &key) {
    if (is_null()) {
        *this = object();
    }
    return as_object()[key];
}

const value &value::operator[](const std::string &key) const {
    try {
        return as_object().at(key);
    } catch (const std::out_of_range &) {
        throw access_error("Key '" + key + "' not found in object.");
    }
}

bool value::operator==(const value &other) const noexcept {
    if (type() != other.type()) {
        return false;
    }

    return m_storage->data == other.m_storage->data;
}

bool value::operator!=(const value &other) const noexcept { return !(*this == other); }

std::ostream &operator<<(std::ostream &os, const value &v) noexcept { return os << v.stringify(); }

} // namespace json
