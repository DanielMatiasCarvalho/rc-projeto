#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <unistd.h>
#include <iostream>

#include "config.hpp"

class Server {
  private:
    std::string _port = DEFAULT_PORT;
    bool _verbose = false;

  public:
    Server(int argc, char **argv);

    /**
     * @brief Displays information about the server.
     */
    void ShowInfo();

    void showMessage(std::string message);
};

#endif