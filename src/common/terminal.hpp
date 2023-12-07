#ifndef __TERMINAL_HPP__
#define __TERMINAL_HPP__

#include <readline/history.h>
#include <readline/readline.h>
#include <stdexcept>
#include <string>

class Terminal {
  public:
    std::string readLine(std::string prompt);
};

#endif