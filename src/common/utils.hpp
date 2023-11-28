#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>
#include <ctime>
#include <iomanip>

bool isNumeric(std::string string);

bool isAlphaNumeric(std::string string);

std::string DateTimeToString(std::time_t value);

#endif