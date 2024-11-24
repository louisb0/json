#pragma once

#include <stdexcept>

namespace json {

class exception : public std::runtime_error {
public:
    explicit exception(const std::string &message);
    explicit exception(const char *message);
};

class parse_error : public exception {
public:
    parse_error(const std::string &message, size_t line);
};

class type_error : public exception {
public:
    type_error(const std::string &expected_type, const std::string &actual_type);

    const std::string &expected_type() const noexcept;
    const std::string &actual_type() const noexcept;

private:
    std::string m_expected_type;
    std::string m_actual_type;
};

class access_error : public exception {
public:
    explicit access_error(const std::string &message);
};

} // namespace json
