/**
 * @file command.hpp
 * @brief Header file for the command program.
 *
 * This file contains the declaration of the classes used in registering and
 * handling a command request.
 */

#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>

#include "config.hpp"
#include "messages.hpp"
#include "protocol.hpp"
#include "server.hpp"

/**
 * @brief Abstract base class for command handlers.
 *
 * This class defines the interface for handling commands received by the
 * server. Subclasses must implement the handle() method to provide specific
 * command handling logic.
 */
class CommandHandler {
  public:
    /**
     * @brief Handles a command message.
     *
     * @param message The command message to handle.
     * @param response The response stream to write the command response to.
     * @param receiver The server instance that received the command.
     */
    virtual void handle(MessageSource &message, std::stringstream &response,
                        Server &receiver) = 0;

    std::string _code; /**< The code associated with the command. */

  protected:
    /**
     * @brief Constructs a CommandHandler object with the specified code.
     *
     * @param code The code associated with the command.
     */
    CommandHandler(std::string code) : _code{code} {}
};

/**
 * @brief Manages command handlers and dispatches commands to the appropriate
 * handler.
 */
class CommandManager {
  private:
    std::unordered_map<std::string, std::shared_ptr<CommandHandler>>
        _handlersUDP = {}; /**< The map of UDP command handlers. */
    std::unordered_map<std::string, std::shared_ptr<CommandHandler>>
        _handlersTCP = {}; /**< The map of TCP command handlers. */

  public:
    /**
     * @brief Registers a command handler.
     *
     * @param handler The command handler to register.
     * @param isTCP Specifies whether the command is for TCP or UDP.
     */
    void registerCommand(std::shared_ptr<CommandHandler> handler, bool isTCP);

    /**
     * @brief Reads and handles a command message.
     *
     * @param message The command message to handle.
     * @param response The response stream to write the command response to.
     * @param receiver The server instance that received the command.
     * @param isTCP Specifies whether the command is for TCP or UDP.
     */
    void readCommand(MessageSource &message, std::stringstream &response,
                     Server &receiver, bool isTCP);
};

/**
 * @brief Command handler for the "LIN" command.
 */
class LoginCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a LoginCommand object.
     */
    LoginCommand() : CommandHandler("LIN"){};

    /**
     * @brief Handles the "LIN" command.
     *
     * @param message The command message to handle.
     * @param response The response stream to write the command response to.
     * @param receiver The server instance that received the command.
     */
    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

/**
 * @brief Command handler for the "LOU" command.
 */
class LogoutCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a LogoutCommand object.
     */
    LogoutCommand() : CommandHandler("LOU"){};

    /**
     * @brief Handles the "LOU" command.
     *
     * @param message The command message to handle.
     * @param response The response stream to write the command response to.
     * @param receiver The server instance that received the command.
     */
    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

/**
 * @brief Command handler for the "UNR" command.
 */
class UnregisterCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs an UnregisterCommand object.
     */
    UnregisterCommand() : CommandHandler("UNR"){};

    /**
     * @brief Handles the "UNR" command.
     *
     * @param message The command message to handle.
     * @param response The response stream to write the command response to.
     * @param receiver The server instance that received the command.
     */
    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

/**
 * @brief Command handler for the "LMA" command.
 */
class ListUserAuctionsCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a ListUserAuctionsCommand object.
     */
    ListUserAuctionsCommand() : CommandHandler("LMA"){};

    /**
     * @brief Handles the "LMA" command.
     *
     * @param message The command message to handle.
     * @param response The response stream to write the command response to.
     * @param receiver The server instance that received the command.
     */
    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

/**
 * @brief Command handler for the "LMB" command.
 */
class ListUserBidsCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a ListUserBidsCommand object.
     */
    ListUserBidsCommand() : CommandHandler("LMB"){};

    /**
     * @brief Handles the "LMB" command.
     *
     * @param message The command message to handle.
     * @param response The response stream to write the command response to.
     * @param receiver The server instance that received the command.
     */
    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

/**
 * @brief Command handler for the "LST" command.
 */
class ListAllAuctionsCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a ListAllAuctionsCommand object.
     */
    ListAllAuctionsCommand() : CommandHandler("LST"){};

    /**
     * @brief Handles the "LST" command.
     *
     * @param message The command message to handle.
     * @param response The response stream to write the command response to.
     * @param receiver The server instance that received the command.
     */
    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

/**
 * @brief Command handler for the "SRC" command.
 */
class ShowRecordCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a ShowRecordCommand object.
     */
    ShowRecordCommand() : CommandHandler("SRC"){};

    /**
     * @brief Handles the "SRC" command.
     *
     * @param message The command message to handle.
     * @param response The response stream to write the command response to.
     * @param receiver The server instance that received the command.
     */
    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

/**
 * @brief Command handler for the "OPA" command.
 */
class OpenCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs an OpenCommand object.
     */
    OpenCommand() : CommandHandler("OPA"){};

    /**
     * @brief Handles the "OPA" command.
     *
     * @param message The command message to handle.
     * @param response The response stream to write the command response to.
     * @param receiver The server instance that received the command.
     */
    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

/**
 * @brief Command handler for the "CLS" command.
 */
class CloseCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a CloseCommand object.
     */
    CloseCommand() : CommandHandler("CLS"){};

    /**
     * @brief Handles the "CLS" command.
     *
     * @param message The command message to handle.
     * @param response The response stream to write the command response to.
     * @param receiver The server instance that received the command.
     */
    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

/**
 * @brief Command handler for the "SAS" command.
 */
class ShowAssetCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a ShowAssetCommand object.
     */
    ShowAssetCommand() : CommandHandler("SAS"){};

    /**
     * @brief Handles the "SAS" command.
     *
     * @param message The command message to handle.
     * @param response The response stream to write the command response to.
     * @param receiver The server instance that received the command.
     */
    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

/**
 * @brief Command handler for the "BID" command.
 */
class BidCommand : public CommandHandler {
  public:
    /**
     * @brief Constructs a BidCommand object.
     */
    BidCommand() : CommandHandler("BID"){};

    /**
     * @brief Handles the "BID" command.
     *
     * @param message The command message to handle.
     * @param response The response stream to write the command response to.
     * @param receiver The server instance that received the command.
     */
    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

/**
 * @brief Writes a protocol error response to the response stream.
 *
 * @param response The response stream to write the error response to.
 */
void protocolError(std::stringstream &response);

#endif