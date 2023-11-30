#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <ctime>
#include <iomanip>
#include <string>

/**
 * @brief Checks if a string contains only numeric characters.
 * 
 * @param string The string to be checked.
 * @return true if the string is numeric, false otherwise.
 */
bool isNumeric(std::string string);

/**
 * @brief Checks if a string contains only alphanumeric characters.
 * 
 * @param string The string to be checked.
 * @return true if the string is alphanumeric, false otherwise.
 */
bool isAlphaNumeric(std::string string);

/**
 * @brief Converts a time value to a string representation.
 * 
 * @param value The time value to be converted.
 * @return The string representation of the time value.
 */
std::string DateTimeToString(std::time_t value);

#endif