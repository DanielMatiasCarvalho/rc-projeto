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