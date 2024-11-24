#include "value.hpp"

namespace json {

value parse(const std::string &json);
value parse_file(const std::string &path);

} // namespace json
