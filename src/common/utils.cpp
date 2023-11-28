#include "utils.hpp"

bool isNumeric(std::string string) {
    for (auto c : string) {
        if (c < '0' || c > '9') {
            return false;
        }
    }

    return true;
}

bool isAlphaNumeric(std::string string) {
    for (auto c : string) {
        if ((c < '0' || c > '9') && (c < 'a' || c > 'z') &&
            (c < 'A' || c > 'Z')) {
            return false;
        }
    }

    return true;
}

std::string DateTimeToString(std::time_t time) {
    std::tm tm = *(std::localtime(&time));
    std::stringstream stream;
    stream << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    std::string string1, string2;
    stream >> string1;
    stream >> string2;
    string1.push_back(' ');
    return string1 + string2;
}