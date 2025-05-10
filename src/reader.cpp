#include "reader.h"

#include <iostream>

void Reader::ReadParams(const std::vector<std::string>& params) {
    values.clear();
    std::string value;
    for (const auto& param : params) {
        std::cout << param << "= ";
        std::getline(std::cin, value);
        values[param] = value;
    }
}

void Reader::ReadHiddenParams(const std::vector<std::string>& params) {
    values.clear();
    std::string value;
    for (const auto& param : params) {
        std::getline(std::cin, value);
        values[param] = value;
    }
}

const std::string& Reader::operator[](const std::string& key) const {
    const std::string empty = "";
    auto it = values.find(key);
    return it != values.end() ? it->second : empty;
}
