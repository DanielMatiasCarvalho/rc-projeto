#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "client.hpp"

class CommandHandler {
  public:
    virtual void handle(std::vector<std::string> args,
                        ClientState receiver) = 0;
    std::string _name;
    std::string _description;
    std::string _usage;
    std::vector<std::string> _alias;

  protected:
    CommandHandler(std::string name, std::string description, std::string usage,
                   std::vector<std::string> alias)
        : _name{name}, _description{description}, _usage{usage}, _alias{alias} {
    }
};

class CommandManager {
  private:
    std::unordered_map<std::string, std::shared_ptr<CommandHandler>> handlers =
        {};

  public:
    void registerCommand(std::shared_ptr<CommandHandler> handler);
    void readCommand(ClientState state);
};

class LoginCommand : public CommandHandler {
  public:
    void handle(std::vector<std::string> args, ClientState receiver);
    LoginCommand()
        : CommandHandler("login", "Logins into the server",
                         "login UID password",
                         std::vector<std::string>({"ln"})){};
};

#endif