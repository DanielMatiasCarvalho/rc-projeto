/**
 * @file command.hpp
 * @brief Header file of classes and related functions for command handling.
 */
#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "client.hpp"
#include "messages.hpp"
#include "utils.hpp"

/**
 * @brief The CommandHandler class is an abstract base class for handling
 * commands.
 *
 * It provides a common interface for handling commands and defines common
 * properties such as name, description, usage, and aliases.
 */
class CommandHandler {
  public:
    /**
     * @brief Handles the command with the given arguments.
     *
     * This method should be implemented by derived classes to provide the
     * specific functionality for handling the command.
     *
     * @param args The arguments passed to the command.
     * @param receiver The client object that receives the command.
     */
    virtual void handle(std::vector<std::string> args, Client &receiver) = 0;

    std::string _name;                // The name of the command
    std::string _description;         // The description of the command
    std::string _usage;               // The usage information of the command
    std::vector<std::string> _alias;  // The aliases of the command

  protected:
    /**
     * @brief Constructs a CommandHandler object with the specified properties.
     *
     * @param name The name of the command.
     * @param description The description of the command.
     * @param usage The usage information of the command.
     * @param alias The aliases of the command.
     */
    CommandHandler(std::string name, std::string description, std::string usage,
                   std::vector<std::string> alias)
        : _name{name},
          _description{description},
          _usage{usage},
          _alias{alias} {}
};

/**
 * @brief The CommandManager class handles the registration and execution of
 * commands.
 */
class CommandManager {
  private:
    std::unordered_map<std::string, std::shared_ptr<CommandHandler>> handlers =
        {};  // The registered command handlers

  public:
    /**
     * @brief Registers a command handler.
     * @param handler The command handler to register.
     */
    void registerCommand(std::shared_ptr<CommandHandler> handler);

    /**
     * @brief Reads and executes a command.
     * @param state The client state.
     */
    void readCommand(std::string command, Client &state);
};

/**
 * @brief Represents a command for logging into the server.
 */
class LoginCommand : public CommandHandler {
  public:
    /**
     * @brief Handles the login command.
     *
     * @param args The arguments passed to the command.
     * @param receiver The client object.
     */
    void handle(std::vector<std::string> args, Client &receiver);

    /**
     * @brief Constructs a LoginCommand object.
     */
    LoginCommand()
        : CommandHandler("login", "Logins into the server",
                         "login UID password", std::vector<std::string>({})){};
};

/**
 * @brief Represents a command for logging out a user from the server.
 */
class LogoutCommand : public CommandHandler {
  public:
    /**
     * @brief Handles the logout command.
     *
     * @param args The command arguments.
     * @param receiver The client object.
     */
    void handle(std::vector<std::string> args, Client &receiver);

    /**
     * @brief Constructs a LogoutCommand object.
     */
    LogoutCommand()
        : CommandHandler("logout", "Logouts user from server", "logout",
                         std::vector<std::string>({})){};
};

/**
 * @brief Represents a command for unregistering the current logged in user in
 * the server.
 */
class UnregisterCommand : public CommandHandler {
  public:
    /**
     * @brief Handles the unregister command.
     *
     * @param args The command arguments.
     * @param receiver The client object.
     */
    void handle(std::vector<std::string> args, Client &receiver);

    /**
     * @brief Constructs an UnregisterCommand object.
     */
    UnregisterCommand()
        : CommandHandler("unregister",
                         "Unregisters current logged in user in server",
                         "unregisters", std::vector<std::string>({})){};
};

/**
 * @brief Represents a command for exiting the program.
 *
 */
class ExitCommand : public CommandHandler {
  public:
    /**
     * @brief Handles the "exit" command.
     *
     * @param args The arguments passed to the command.
     * @param receiver The Client object that will handle the command.
     */
    void handle(std::vector<std::string> args, Client &receiver);

    /**
     * @brief Constructs an ExitCommand object.
     *
     */
    ExitCommand()
        : CommandHandler("exit", "Unregisters current logged in user in server",
                         "exit", std::vector<std::string>({})){};
};

/**
 * @brief The OpenCommand class represents a command that opens a new auction.
 */
class OpenCommand : public CommandHandler {
  public:
    /**
     * @brief Handles the open command.
     *
     * @param args The arguments passed to the command.
     * @param receiver The client object that will handle the command.
     */
    void handle(std::vector<std::string> args, Client &receiver);

    /**
     * @brief Constructs an OpenCommand object.
     */
    OpenCommand()
        : CommandHandler("open", "Opens a new auction",
                         "open name asset_fname start_value timeactive",
                         std::vector<std::string>({})){};
};

/**
 * @brief Represents a command to close an existing auction with AID identifier.
 */
class CloseCommand : public CommandHandler {
  public:
    /**
     * @brief Handles the close command.
     *
     * @param args The command arguments.
     * @param receiver The client object.
     */
    void handle(std::vector<std::string> args, Client &receiver);

    /**
     * @brief Constructs a CloseCommand object.
     */
    CloseCommand()
        : CommandHandler("close",
                         "Close an existing auction with AID identifier",
                         "close AID", std::vector<std::string>({})){};
};

/**
 * @brief A command handler for listing the auctions started by the logged in
 * user.
 */
class ListUserAuctionsCommand : public CommandHandler {
  public:
    /**
     * @brief Handles the command to list user auctions.
     *
     * @param args The command arguments.
     * @param receiver The client object.
     */
    void handle(std::vector<std::string> args, Client &receiver);

    /**
     * @brief Constructs a ListUserAuctionsCommand object.
     */
    ListUserAuctionsCommand()
        : CommandHandler("myauctions",
                         "List the auctions started by the logged in user",
                         "myauctions", std::vector<std::string>({"ma"})){};
};

/**
 * @brief Represents a command handler for listing the bids started by the
 * logged in user.
 */
class ListUserBidsCommand : public CommandHandler {
  public:
    /**
     * @brief Handles the execution of the command.
     *
     * @param args The command arguments.
     * @param receiver The client object.
     */
    void handle(std::vector<std::string> args, Client &receiver);

    /**
     * @brief Constructs a new ListUserBidsCommand object.
     */
    ListUserBidsCommand()
        : CommandHandler("mybids",
                         "List the bids started by the logged in user",
                         "mybids", std::vector<std::string>({"mb"})){};
};

/**
 * @brief A command handler for listing all auctions.
 */
class ListAllAuctionsCommand : public CommandHandler {
  public:
    /**
     * @brief Handles the execution of the command.
     *
     * @param args The command arguments.
     * @param receiver The client object.
     */
    void handle(std::vector<std::string> args, Client &receiver);

    /**
     * @brief Constructs a new ListAllAuctionsCommand object.
     */
    ListAllAuctionsCommand()
        : CommandHandler("list", "List all the current active auctions", "list",
                         std::vector<std::string>({"l"})){};
};

/**
 * @brief Represents a command that requests the server to send the asset file
 * of an auction.
 */
class ShowAssetCommand : public CommandHandler {
  public:
    /**
     * @brief Handles the execution of the command.
     *
     * @param args The command arguments.
     * @param receiver The client object.
     */
    void handle(std::vector<std::string> args, Client &receiver);

    /**
     * @brief Constructs a ShowAssetCommand object.
     */
    ShowAssetCommand()
        : CommandHandler(
              "show_asset",
              "The user requests the server the asset file of an auction",
              "show_asset AID", std::vector<std::string>({"sa"})){};
};

/**
 * @brief The BidCommand class represents a command that allows the user to bid
 * a certain value in a certain auction.
 */
class BidCommand : public CommandHandler {
  public:
    /**
     * @brief Handles the bid command.
     *
     * @param args The arguments passed to the command.
     * @param receiver The client object that will handle the bid.
     */
    void handle(std::vector<std::string> args, Client &receiver);

    /**
     * @brief Constructs a new BidCommand object.
     */
    BidCommand()
        : CommandHandler("bid",
                         "The user bids a certain value in a certain auction",
                         "bid AID value", std::vector<std::string>({"b"})){};
};

/**
 * @brief A command handler for showing all the auction details.
 */
class ShowRecordCommand : public CommandHandler {
  public:
    /**
     * @brief Handles the show_record command.
     *
     * @param args The command arguments.
     * @param receiver The client object.
     */
    void handle(std::vector<std::string> args, Client &receiver);

    /**
     * @brief Constructs a ShowRecordCommand object.
     */
    ShowRecordCommand()
        : CommandHandler("show_record", "Show all the auction details",
                         "show_record AID", std::vector<std::string>({"sr"})){};
};

/**
 * @brief Exception class for command-related errors.
 *
 * This class is derived from std::runtime_error and is used to represent
 * exceptions that occur during command execution.
 */
class CommandException : public std::runtime_error {
  public:
    /**
     * @brief Constructs a CommandException object with the specified reason.
     *
     * @param reason The reason for the exception.
     */
    CommandException(std::string reason)
        : std::runtime_error("ERROR: " + reason){};
};

/**
 * @brief Exception thrown when invalid arguments are provided for a command.
 */
class CommandArgumentException : public CommandException {
  public:
    /**
     * @brief Constructs a CommandArgumentException with the specified usage
     * information.
     *
     * @param usage The usage information for the command.
     */
    CommandArgumentException(std::string usage)
        : CommandException("Invalid arguments.\nUsage: " + usage){};
};

/**
 * @brief Exception thrown when an unknown command is encountered.
 */
class UnknownCommandException : public CommandException {
  public:
    UnknownCommandException() : CommandException("Unknown Command."){};
};

#endif