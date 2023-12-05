#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <signal.h>
#include <unistd.h>
#include <iostream>

#include "command.hpp"
#include "config.hpp"
#include "database.hpp"
#include "network.hpp"

/**
 * @class Server
 * @brief Represents a server object.
 */

class Server {
  private:
    std::string _port = DEFAULT_PORT; /**< The port number. */
    bool _verbose =
        false; /**< Flag indicating whether to display verbose output. */

  public:
    /**
         * @brief Constructs a Server object.
         * @param argc The number of command-line arguments.
         * @param argv The command-line arguments.
         */
    Server(int argc, char **argv);

    /**
         * @brief Displays information about the server.
         */
    void ShowInfo();

    /**
         * @brief Displays a message.
         * @param message The message to display.
         */
    void showMessage(std::string message);

    /**
 * @brief Get the port number.
 * 
 * @return std::string The port number.
 */
    std::string getPort() { return _port; }
};

#endif