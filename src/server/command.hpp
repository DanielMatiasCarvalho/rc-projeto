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


class CommandHandler {
  public:

    virtual void handle(MessageSource &message, std::stringstream &response,
                        Server &receiver) = 0;

    std::string _code; 

  protected:

    CommandHandler(std::string code) : _code{code} {}
};


class CommandManager {
  private:
    std::unordered_map<std::string, std::shared_ptr<CommandHandler>>
        _handlersUDP = {};
    std::unordered_map<std::string, std::shared_ptr<CommandHandler>>
        _handlersTCP = {};

  public:

    void registerCommand(std::shared_ptr<CommandHandler> handler, bool isTCP);

    void readCommand(MessageSource &message, std::stringstream &response,
                     Server &receiver, bool isTCP);
};

class LoginCommand : public CommandHandler {
  public:

    LoginCommand() : CommandHandler("LIN"){};

    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

class LogoutCommand : public CommandHandler {
  public:

    LogoutCommand() : CommandHandler("LOU"){};

    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

class UnregisterCommand : public CommandHandler {
  public:

    UnregisterCommand() : CommandHandler("UNR"){};

    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

class ListUserAuctionsCommand : public CommandHandler {
  public:

    ListUserAuctionsCommand() : CommandHandler("LMA"){};

    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

class ListUserBidsCommand : public CommandHandler {
  public:

    ListUserBidsCommand() : CommandHandler("LMB"){};

    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

class ListAllAuctionsCommand : public CommandHandler {
  public:

    ListAllAuctionsCommand() : CommandHandler("LST"){};

    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};


class ShowRecordCommand : public CommandHandler {
  public:

    ShowRecordCommand() : CommandHandler("SRC"){};

    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

class OpenCommand : public CommandHandler {
  public:

    OpenCommand() : CommandHandler("OPA"){};

    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

class CloseCommand : public CommandHandler {
  public:

    CloseCommand() : CommandHandler("CLS"){};

    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

class ShowAssetCommand : public CommandHandler {
  public:

    ShowAssetCommand() : CommandHandler("SAS"){};

    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};
class BidCommand : public CommandHandler {
  public:

    BidCommand() : CommandHandler("BID"){};

    void handle(MessageSource &message, std::stringstream &response,
                Server &receiver);
};

void protocolError(std::stringstream &response);
#endif