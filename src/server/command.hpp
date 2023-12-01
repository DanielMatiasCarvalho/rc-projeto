#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>

#include "config.hpp"
#include "protocol.hpp"
#include "server.hpp"

class CommandHandler {
  public:
    virtual void handle(std::stringstream &message, Server &receiver) = 0;

    std::string _code;
    std::string _description;
    std::string _usage;

  protected:
    CommandHandler(std::string code, std::string description, std::string usage)
        : _code{code}, _description{description}, _usage{usage} {}
};

class CommandManager {
  private:
    std::unordered_map<std::string, std::shared_ptr<CommandHandler>> _handlers =
        {};

  public:
    void registerHandler(std::shared_ptr<CommandHandler> handler);

    void readCommand(std::stringstream message, Server &receiver);
};

#endif