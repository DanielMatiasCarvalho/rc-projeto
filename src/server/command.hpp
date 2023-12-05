#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>

#include "config.hpp"
#include "protocol.hpp"

class Server;

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
    void registerCommand(std::shared_ptr<CommandHandler> handler);

    /**
     * @brief Reads and executes a command from the given message.
     * @param message The message containing the command.
     * @param receiver The server object that will receive the command.
     */
    void readCommand(std::stringstream &message, Server &receiver);
};

/**
 * @brief Represents a login command handler.
 * 
 * This class handles the login command and is responsible for processing the message
 * received from the client and performing the necessary actions on the server.
 * 
 */
class LoginCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a LoginCommand object.
     */
    LoginCommand() : CommandHandler("LIN"){};

    /**
     * @brief Handles the login command.
     * 
     * This function is called when a login command is received from the client. It processes
     * the message and performs the necessary actions on the server.
     * 
     * @param message The message containing the command parameters.
     * @param receiver The server object that will handle the command.
     */
    void handle(std::stringstream &message, Server &receiver);
};

/**
 * @brief Represents a command for logging out a user.
 * 
 * This class handles the logout command and is responsible for processing the message
 * received from the client and performing the necessary actions on the server.
 * 
 */
class LogoutCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a LogoutCommand object.
     */
    LogoutCommand() : CommandHandler("LOU"){};

    /**
     * @brief Handles the logout command.
     * 
     * This function is called when a logout command is received from the client. It processes
     * the message and performs the necessary actions on the server.
     * 
     * @param message The message containing the command parameters.
     * @param receiver The server object that will handle the command.
     */
    void handle(std::stringstream &message, Server &receiver);
};

/**
 * @brief Represents a command handler for unregistering a command.
 * 
 * This class handles the unregister command and is responsible for processing the message
 * received from the client and performing the necessary actions on the server.
 * 
 */
class UnregisterCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs an UnregisterCommand object.
     */
    UnregisterCommand() : CommandHandler("UNR"){};

    /**
     * @brief Handles the unregister command.
     * 
     * This function is called when a unregister command is received from the client. It processes
     * the message and performs the necessary actions on the server.
     * 
     * @param message The message containing the command parameters.
     * @param receiver The server object that receives the command.
     */
    void handle(std::stringstream &message, Server &receiver);
};

/**
 * @brief Represents a command handler for listing the auctions owned by the user.
 * 
 * This class handles the list user auctions command and is responsible for processing the message
 * received from the client and performing the necessary actions on the server.
 * 
 */
class ListUserAuctionsCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a ListUserAuctionsCommand object.
     */
    ListUserAuctionsCommand() : CommandHandler("LMA"){};

    /**
     * @brief Handles the list my auctions command.
     * 
     * This function is called when a list user auctions command is received from the client. It processes
     * the message and performs the necessary actions on the server.
     * 
     * @param message The message containing the command parameters.
     * @param receiver The server object that receives the command.
     */
    void handle(std::stringstream &message, Server &receiver);
};

/**
 * @brief Represents a command handler for listing the user's bids.
 * 
 * This class handles the list my bids command and is responsible for processing the message
 * received from the client and performing the necessary actions on the server.
 * 
 */
class ListUserBidsCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a ListUserBidsCommand object.
     */
    ListUserBidsCommand() : CommandHandler("LMB"){};

    /**
     * @brief Handles the list my bids command.
     * 
     * This function is called when a list user bids command is received from the client. It processes
     * the message and performs the necessary actions on the server.
     * 
     * @param message The message containing the command parameters.
     * @param receiver The server object that receives the command.
     */
    void handle(std::stringstream &message, Server &receiver);
};

/**
 * @brief Represents a command handler for the list command.
 * 
 * This class handles the list all auctions command and is responsible for processing the message
 * received from the client and performing the necessary actions on the server.
 * 
 */
class ListAllAuctionsCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a ListCommand object.
     */
    ListAllAuctionsCommand() : CommandHandler("LST"){};

    /**
     * @brief Handles the list command.
     * 
     * This function is called when a list all auctions command is received from the client. It processes
     * the message and performs the necessary actions on the server.
     * 
     * @param message The command message received from the client.
     * @param receiver The server object that receives the command.
     */
    void handle(std::stringstream &message, Server &receiver);
};

/**
 * @brief Represents a command handler for showing a record.
 * 
 * This class handles the show record command and is responsible for processing the message
 * received from the client and performing the necessary actions on the server.
 * 
 */
class ShowRecordCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a ShowRecordCommand object.
     */
    ShowRecordCommand() : CommandHandler("SRC"){};

    /**
     * @brief Handles the command to show a record.
     * 
     * This function is called when a show record command is received from the client. It processes
     * the message and performs the necessary actions on the server.
     * 
     * @param message The message containing the command parameters.
     * @param receiver The server object that receives the command.
     */
    void handle(std::stringstream &message, Server &receiver);
};

/**
 * @brief Represents a command handler for the open command.
 * 
 * This class handles the open command and is responsible for processing the message
 * received from the client and performing the necessary actions on the server.
 * 
 */
class OpenCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs an OpenCommand object.
     */
    OpenCommand() : CommandHandler("OPA"){};

    /**
     * @brief Handles the open command.
     * 
     * This function is called when a show record command is received from the client. It processes
     * the message and performs the necessary actions on the server.
     * 
     * @param message The message containing the command parameters.
     * @param receiver The server object that receives the command.
     */
    void handle(std::stringstream &message, Server &receiver);
};

/**
 * @brief Represents a command for closing a connection.
 * 
 * This class handles the close command and is responsible for processing the message
 * received from the client and performing the necessary actions on the server.
 * 
 */
class CloseCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a CloseCommand object.
     */
    CloseCommand() : CommandHandler("CLS"){};

    /**
     * @brief Handles the close command.
     * 
     * This function is called when a close command is received from the client. It processes
     * the message and performs the necessary actions on the server.
     * 
     * @param message The message containing the command.
     * @param receiver The server object that receives the command.
     */
    void handle(std::stringstream &message, Server &receiver);
};

/**
 * @brief Represents a command handler for showing an asset.
 * 
 * This class handles the show asset command and is responsible for processing the message
 * received from the client and performing the necessary actions on the server.
 * 
 */
class ShowAssetCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a ShowAssetCommand object.
     */
    ShowAssetCommand() : CommandHandler("SAS"){};

    /**
     * @brief Handles the command to show an asset.
     * 
     * This function is called when a show asset command is received from the client. It processes
     * the message and performs the necessary actions on the server.
     * 
     * @param message The message containing the command parameters.
     * @param receiver The server object that receives the command.
     */
    void handle(std::stringstream &message, Server &receiver);
};

/**
 * @brief Represents a command handler for the bid command.
 * 
 * This class handles the bid command and is responsible for processing the message
 * received from the client and performing the necessary actions on the server.
 * 
 */
class BidCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a BidCommand object.
     */
    BidCommand() : CommandHandler("BID"){};

    /**
     * @brief Handles the bid command.
     * 
     * This function is called when a bid command is received from the client. It processes
     * the message and performs the necessary actions on the server.
     * 
     * @param message The message containing the command arguments.
     * @param receiver The server object that receives the command.
     */
    void handle(std::stringstream &message, Server &receiver);
};
#endif