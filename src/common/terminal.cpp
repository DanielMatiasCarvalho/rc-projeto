/**
 * @file terminal.cpp
 * @brief Implementation of the Terminal readLine.
 */
#include "terminal.hpp"

std::string Terminal::readLine(std::string prompt) {
    //Read a line from the terminal, adding the prompt
    char *line = readline(prompt.c_str());

    if (line == NULL) { //If the user pressed Ctrl+D
        throw std::runtime_error("readline() failed");
    }

    std::string result(line); //Convert the line to a string

    add_history(line); //Add the line to the history

    free(line); //Free the memory allocated by readline()

    return result;
}