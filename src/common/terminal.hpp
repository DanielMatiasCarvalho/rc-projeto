#ifndef __TERMINAL_HPP__
#define __TERMINAL_HPP__

#include <string>
#include <stdexcept>
#include <readline/readline.h>
#include <readline/history.h>

class Terminal {
    public:
        std::string readLine(std::string prompt);
};

#endif