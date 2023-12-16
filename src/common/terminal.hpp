/**
 * @file terminal.hpp
 * @brief This file contains the class Terminal, which allows
 *        our client to behave like a typical terminal.
 */
#ifndef __TERMINAL_HPP__
#define __TERMINAL_HPP__

#include <readline/history.h>
#include <readline/readline.h>
#include <stdexcept>
#include <string>

/**
 * @brief The Terminal class provides functionality for reading input from the
 * terminal.
 */
class Terminal {
  public:
    /**
     * @brief Reads a line of input from the terminal.
     * @param prompt The prompt to display before reading the input.
     * @return The line of input entered by the user.
     */
    std::string readLine(std::string prompt);
};

#endif