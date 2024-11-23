#include "json_value.hpp"
#include "parser.hpp"

#include <iostream>

int main() {
    const char *src = R"({"test": { "bool": true}, "arr": [ 1, 2, 3 ]})";

    json::JSONValue json = json::parse(src);

    std::cout << json["test"].string();
    for (int i : json["arr"]) {
        std::cout << i;
    }
}
