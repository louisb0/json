#pragma once

#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>

namespace json {

class JSONValue {
public:
    enum Type {
        NONE,
        BOOLEAN,
        NUMBER,
        STRING,
        ARRAY,
        OBJECT,
    } type;

    union Data {
        bool boolean;
        int number;
        std::string *string;
        std::vector<JSONValue> *array;
        std::unordered_map<std::string, JSONValue> *object;

        Data() : boolean(false) {}
    } data;

    JSONValue() : type(NONE) {}
    explicit JSONValue(bool boolean) : type(BOOLEAN) { data.boolean = boolean; }
    explicit JSONValue(int number) : type(NUMBER) { data.number = number; }
    explicit JSONValue(std::string *string) : type(STRING) { data.string = string; }
    explicit JSONValue(std::vector<JSONValue> *array) : type(ARRAY) { data.array = array; }
    explicit JSONValue(std::unordered_map<std::string, JSONValue> *object) : type(OBJECT) {
        data.object = object;
    }

    // TODO(louis): user error(s) should not be asserted
    operator int() const {
        assert(type == NUMBER);
        return data.number;
    }

    JSONValue &operator[](const char *key) { return operator[](std::string(key)); }
    JSONValue &operator[](const std::string &key) {
        assert(type == OBJECT);
        auto it = data.object->find(key);
        assert(it != data.object->end());
        return it->second;
    }

    auto begin() {
        assert(type == ARRAY);
        return data.array->begin();
    }

    auto end() {
        assert(type == ARRAY);
        return data.array->end();
    }

    std::string string() {
        switch (type) {
        case NONE:
            return "null";

        case BOOLEAN:
            return data.boolean ? "true" : "false";

        case NUMBER:
            return std::to_string(data.number);

        case STRING:
            return "\"" + *data.string + "\"";

        case ARRAY: {
            std::string result = "[";
            for (size_t i = 0; i < data.array->size(); i++) {
                if (i > 0)
                    result += ",";

                result += (*data.array)[i].string();
            }
            return result + "]";
        }

        case OBJECT: {
            std::string result = "{";

            bool first = true;
            for (auto &pair : *data.object) {
                if (!first)
                    result += ",";

                result += "\"" + pair.first + "\":" + pair.second.string();
                first = false;
            }

            return result + "}";
        }
        }

        assert(!"Unreachable");
    }
};

} // namespace json
