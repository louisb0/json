#include "json/exception.hpp"

namespace json {

exception::exception(const std::string &message) : std::runtime_error(message) {}
exception::exception(const char *message) : std::runtime_error(message) {}

type_error::type_error(const std::string &expected_type, const std::string &actual_type)
    : exception("Attempted to read '" + actual_type + "' + as '" + expected_type + "'"),
      m_expected_type(expected_type), m_actual_type(actual_type) {}
const std::string &type_error::expected_type() const noexcept { return m_expected_type; }
const std::string &type_error::actual_type() const noexcept { return m_actual_type; }

access_error::access_error(const std::string &message) : exception(message) {}

} // namespace json
