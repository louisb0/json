#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace json {

template <typename T> class non_owning {
    T *ptr;

public:
    non_owning(T *p = nullptr) noexcept : ptr(p) {}

    T *get() const noexcept { return ptr; }
    T &operator*() const noexcept { return *ptr; }
    T *operator->() const noexcept { return ptr; }
    explicit operator bool() const noexcept { return ptr != nullptr; }
};

enum class value_t { Null, Boolean, Number, String, Array, Object };

class value {
public:
    [[nodiscard]] value();
    [[nodiscard]] value(std::nullptr_t);
    [[nodiscard]] value(bool value);
    [[nodiscard]] value(int value);
    [[nodiscard]] value(double value);
    [[nodiscard]] value(const char *value);
    [[nodiscard]] value(std::string value);
    [[nodiscard]] value(std::vector<value> values);
    [[nodiscard]] value(std::unordered_map<std::string, value> values);
    [[nodiscard]] value(std::initializer_list<std::pair<std::string, value>> init);

    value(const value &other);
    value &operator=(const value &other);
    value(value &&other) noexcept;
    value &operator=(value &&other) noexcept;
    ~value();

    static value array();
    static value array(std::vector<value> values);

    [[nodiscard]] value_t type() const noexcept;
    [[nodiscard]] bool is_null() const noexcept;
    [[nodiscard]] bool is_boolean() const noexcept;
    [[nodiscard]] bool is_number() const noexcept;
    [[nodiscard]] bool is_string() const noexcept;
    [[nodiscard]] bool is_array() const noexcept;
    [[nodiscard]] bool is_object() const noexcept;

    [[nodiscard]] bool as_boolean() const;
    [[nodiscard]] double as_number() const;
    [[nodiscard]] const std::string &as_string() const;
    [[nodiscard]] const std::vector<value> &as_array() const;
    [[nodiscard]] const std::unordered_map<std::string, value> &as_object() const;

    [[nodiscard]] std::string &as_string();
    [[nodiscard]] std::vector<value> &as_array();
    [[nodiscard]] std::unordered_map<std::string, value> &as_object();

    [[nodiscard]] std::optional<bool> try_boolean() const noexcept;
    [[nodiscard]] std::optional<double> try_number() const noexcept;
    [[nodiscard]] non_owning<const std::string> try_string() const noexcept;
    [[nodiscard]] non_owning<const std::vector<value>> try_array() const noexcept;
    [[nodiscard]] non_owning<const std::unordered_map<std::string, value>>
    try_object() const noexcept;

    value &operator[](size_t index);
    const value &operator[](size_t index) const;
    value &operator[](const std::string &key);
    const value &operator[](const std::string &key) const;

    bool operator==(const value &other) const noexcept;
    bool operator!=(const value &other) const noexcept;

private:
    struct storage;
    std::unique_ptr<storage> m_storage;
};

inline value object(std::initializer_list<std::pair<std::string, value>> init) {
    return value(init);
}
inline value array() { return value::array(); }
inline value array(std::initializer_list<value> init) {
    return value::array(std::vector<value>(init));
}
inline value null() { return value(nullptr); }
inline value boolean(bool val) { return value(val); }
inline value number(int val) { return value(val); }
inline value number(double val) { return value(val); }
inline value string(const char *val) { return value(val); }
inline value string(std::string val) { return value(std::move(val)); }

} // namespace json
