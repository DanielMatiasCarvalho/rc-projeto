#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>

#include "config.hpp"
#include "protocol.hpp"
#include "server.hpp"

/**
 * @brief The base class for command handlers.
 * 
 * This class provides an interface for handling commands and defines a common member variable.
 */
class CommandHandler {
  public:
    /**
     * @brief Handles the command.
     * 
     * This pure virtual function must be implemented by derived classes to handle the command.
     * 
     * @param message The command message.
     * @param receiver The server object that receives the command.
     */
    virtual void handle(std::stringstream &message, Server &receiver) = 0;

    std::string _code; /**< The code associated with the command. */

  protected:
    /**
     * @brief Constructs a CommandHandler object.
     * 
     * @param code The code associated with the command.
     */
    CommandHandler(std::string code) : _code{code} {}
};

/**
 * @brief The CommandManager class handles the registration and execution of command handlers.
 */
class CommandManager {
  private:
    std::unordered_map<std::string, std::shared_ptr<CommandHandler>> _handlers =
        {};

  public:
    /**
     * @brief Registers a command handler.
     * @param handler The command handler to register.
     */
    void registerHandler(std::shared_ptr<CommandHandler> handler);

    /**
     * @brief Reads and executes a command from the given message.
     * @param message The message containing the command.
     * @param receiver The server object that will receive the command.
     */
    void readCommand(std::stringstream message, Server &receiver);
};

#endif