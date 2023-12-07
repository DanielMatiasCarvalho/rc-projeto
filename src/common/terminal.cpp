#include "terminal.hpp"

std::string Terminal::readLine(std::string prompt) {
    char *line = readline(prompt.c_str());

    if (line == NULL) {
        throw std::runtime_error("readline() failed");
    }

    std::string result(line);

    add_history(line);

    free(line);

    return result;
}