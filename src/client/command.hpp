#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "client.hpp"
#include "utils.hpp"

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
                         "login UID password", std::vector<std::string>({})){};
};

class LogoutCommand : public CommandHandler {
  public:
    void handle(std::vector<std::string> args, ClientState receiver);
    LogoutCommand()
        : CommandHandler("logout", "Logouts user from server", "logout",
                         std::vector<std::string>({})){};
};

class UnregisterCommand : public CommandHandler {
  public:
    void handle(std::vector<std::string> args, ClientState receiver);
    UnregisterCommand()
        : CommandHandler("unregister",
                         "Unregisters current logged in user in server",
                         "unregisters", std::vector<std::string>({})){};
};

class ExitCommand : public CommandHandler {
  public:
    void handle(std::vector<std::string> args, ClientState receiver);
    ExitCommand()
        : CommandHandler("exit", "Unregisters current logged in user in server",
                         "exit", std::vector<std::string>({})){};
};

class OpenCommand : public CommandHandler {
  public:
    void handle(std::vector<std::string> args, ClientState receiver);
    OpenCommand()
        : CommandHandler("open", "Opens a new auction",
                         "open name asset_fname start_value timeactive",
                         std::vector<std::string>({})){};
};

class CloseCommand : public CommandHandler {
  public:
    void handle(std::vector<std::string> args, ClientState receiver);
    CloseCommand()
        : CommandHandler("close",
                         "Close an existing auction with AID identifier",
                         "close AID", std::vector<std::string>({})){};
};

class ListUserAuctionsCommand : public CommandHandler {
  public:
    void handle(std::vector<std::string> args, ClientState receiver);
    ListUserAuctionsCommand()
        : CommandHandler("myauctions",
                         "List the auctions started by the logged in user",
                         "myauctions", std::vector<std::string>({"ma"})){};
};

class ListUserBidsCommand : public CommandHandler {
  public:
    void handle(std::vector<std::string> args, ClientState receiver);
    ListUserBidsCommand()
        : CommandHandler("mybids",
                         "List the bids started by the logged in user",
                         "mybids", std::vector<std::string>({"mb"})){};
};

class ListAllAuctionsCommand : public CommandHandler {
  public:
    void handle(std::vector<std::string> args, ClientState receiver);
    ListAllAuctionsCommand()
        : CommandHandler("list", "List all the current active auctions", "list",
                         std::vector<std::string>({"l"})){};
};

class ShowAssetCommand : public CommandHandler {
  public:
    void handle(std::vector<std::string> args, ClientState receiver);
    ShowAssetCommand()
        : CommandHandler(
              "show_asset",
              "The user requests the server the asset file of an auction",
              "show_asset AID", std::vector<std::string>({"sa"})){};
};

class BidCommand : public CommandHandler {
  public:
    void handle(std::vector<std::string> args, ClientState receiver);
    BidCommand()
        : CommandHandler("bid",
                         "The user bids a certain value in a certain auction",
                         "bid AID value", std::vector<std::string>({"b"})){};
};

class ShowRecordCommand : public CommandHandler {
  public:
    void handle(std::vector<std::string> args, ClientState receiver);
    ShowRecordCommand()
        : CommandHandler("show_record", "Show all the auction details",
                         "show_record AID", std::vector<std::string>({"sr"})){};
};

class CommandException : public std::runtime_error {
  public:
    CommandException(std::string reason)
        : std::runtime_error("ERROR: " + reason){};
};

class CommandArgumentException : public CommandException {
  public:
    CommandArgumentException(std::string usage)
        : CommandException("Invalid arguments.\nUsage: " + usage){};
};

class UnknownCommandException : public CommandException {
  public:
    UnknownCommandException() : CommandException("Unknown Command."){};
};

#endif