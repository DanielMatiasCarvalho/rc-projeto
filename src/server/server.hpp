/**
 * @file server.hpp
 * @brief Header file for the server program.
 *
 * This file contains the class Server, used to represent a server object
 */
#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <iostream>
#include <memory>

#include <signal.h>
#include <unistd.h>

#include "config.hpp"
#include "database.hpp"
#include "network.hpp"

/**
 * @brief  Strategy class that represents the loggin process on verbose case.
 * Can be extended for more functionallity (like different logging methods).
 */
class Logger {
  private:
    std::vector<std::string> _messages;

  public:
    /**
     * @brief  Adds message to the log queue.
     * @param  message message
     */
    void log(std::string message);

    /**
     * @brief  Pushes the queue of messages to the logging target.
     */
    void push();

    /**
     * @brief  Adds a message to the queue and immediatly pushes the queue.
     * @param  message:
     */
    void logPush(std::string message);
};

/**
 * @class Server
 * @brief Represents a server object.
 */
class Server {
  private:
    std::string _port = DEFAULT_PORT; /**< The port number. */
    bool _verbose =
        false; /**< Flag indicating whether to display verbose output. */
    std::vector<std::shared_ptr<Logger>> _loggers;

  public:
    std::unique_ptr<Database> _database; /**< The database. */
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

    void log(std::string message);

    void push();

    void logPush(std::string message);

    /**
     * @brief Get the port number.
     *
     * @return std::string The port number.
     */
    std::string getPort() { return _port; }
};

#endif