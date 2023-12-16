/**
 * @file utils.cpp
 * @brief Implementation of the utility functions.
 */
#include "utils.hpp"

bool isNumeric(std::string string) {
    for (auto c : string) {
        // For each character in the string, check if it is a digit
        if (c < '0' || c > '9') {
            return false;
        }
    }

    return true;
}

bool isAlphaNumeric(std::string string) {
    for (auto c : string) {
        // For each character in the string, check if it is a digit or a letter
        if ((c < '0' || c > '9') && (c < 'a' || c > 'z') &&
            (c < 'A' || c > 'Z')) {
            return false;
        }
    }

    return true;
}

std::string DateTimeToString(std::time_t time) {
    std::tm tm =
        *(std::localtime(&time));  // Convert the time value to a tm struct
    std::stringstream stream;
    // Convert the tm struct to a string of the given format, putting it in the
    // stream
    stream << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    std::string string1, string2;
    stream >> string1;       // Get the first part of the string (date)
    stream >> string2;       // Get the second part of the string (time)
    string1.push_back(' ');  // Add a space between the date and time
    return string1 + string2;
}