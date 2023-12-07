#include "command.hpp"

/**
 * Registers a command handler.
 *
 * @param handler The command handler to register.
 */
void CommandManager::registerCommand(std::shared_ptr<CommandHandler> handler) {
    this->_handlers.insert({handler->_code, handler});
}

/**
 * @brief Reads and processes a command from a given message.
 * 
 * This function takes a stringstream message as input and processes the command contained in it.
 * The processed command is then executed on the correct handle function, which Server object is 
 * passed as a reference.
 * 
 * @param message The stringstream containing the command message.
 * @param receiver The Server object that receives the command.
 */
void CommandManager::readCommand(std::stringstream &message,
                                 std::stringstream &response,
                                 Server &receiver) {
    std::string code;
    for (size_t i = 0; i < 3; i++) {  //Reads the 3 digit code from the message
        char c = (char)message.get();
        code.push_back(c);
    }

    auto handler =
        this->_handlers.find(code);  //Finds the correct handler for the command
    if (handler ==
        this->_handlers
            .end()) {  //If the handler is not found, the command is not valid
        std::string str = PROTOCOL_ERROR_IDENTIFIER;
        for (auto c : str) {
            response.put(c);
        }
        response.put('\n');
        return;
    }

    handler->second->handle(
        message, response,
        receiver);  //Executes the command on the correct handler
}

void LoginCommand::handle(std::stringstream &message,
                          std::stringstream &response, Server &receiver) {

    LoginCommunication loginCommunication;
    try {
        loginCommunication.decodeRequest(message);

        if (!receiver._database->loginUser(loginCommunication._uid,
                                           loginCommunication._password)) {
            loginCommunication._status = "OK";
        } else {
            loginCommunication._status = "REG";
        }
    } catch (LoginException const &e) {
        loginCommunication._status = "NOK";
    } catch (ProtocolException const &e) {
        loginCommunication._status = "ERR";
    }
    response = loginCommunication.encodeResponse();
}

void LogoutCommand::handle(std::stringstream &message,
                           std::stringstream &response, Server &receiver) {
    LogoutCommunication logoutCommunication;
    try {
        logoutCommunication.decodeRequest(message);
        receiver._database->logoutUser(logoutCommunication._uid,
                                       logoutCommunication._password);
        logoutCommunication._status = "OK";
    } catch (LoginException const &e) {
        logoutCommunication._status = "NOK";
    } catch (UnregisteredException const &e) {
        logoutCommunication._status = "UNR";
    } catch (ProtocolException const &e) {
        logoutCommunication._status = "ERR";
    }
    response = logoutCommunication.encodeResponse();
}

void UnregisterCommand::handle(std::stringstream &message,
                               std::stringstream &response, Server &receiver) {
    UnregisterCommunication unregisterCommunication;
    try {
        unregisterCommunication.decodeRequest(message);
        receiver._database->logoutUser(unregisterCommunication._uid,
                                       unregisterCommunication._password);
        unregisterCommunication._status = "OK";
    } catch (LoginException const &e) {
        unregisterCommunication._status = "NOK";
    } catch (UnregisteredException const &e) {
        unregisterCommunication._status = "UNR";
    } catch (ProtocolException const &e) {
        unregisterCommunication._status = "ERR";
    }
    response = unregisterCommunication.encodeResponse();
}

void ListUserAuctionsCommand::handle(std::stringstream &message,
                                     std::stringstream &response,
                                     Server &receiver) {

    ListUserAuctionsCommunication listUserAuctionsCommunication;
    try {
        listUserAuctionsCommunication.decodeRequest(message);
        listUserAuctionsCommunication._auctions =
            receiver._database->getUserAuctions(
                listUserAuctionsCommunication._uid);
        if (listUserAuctionsCommunication._auctions.empty()) {
            listUserAuctionsCommunication._status = "NOK";
        } else {
            listUserAuctionsCommunication._status = "OK";
        }
    } catch (LoginException const &e) {
        listUserAuctionsCommunication._status = "NLG";
    } catch (ProtocolException const &e) {
        listUserAuctionsCommunication._status = "ERR";
    }
    response = listUserAuctionsCommunication.encodeResponse();
}

void ListUserBidsCommand::handle(std::stringstream &message,
                                 std::stringstream &response,
                                 Server &receiver) {

    ListUserBidsCommunication listUserBidsCommunication;
    try {
        listUserBidsCommunication.decodeRequest(message);
        listUserBidsCommunication._bids =
            receiver._database->getUserBids(listUserBidsCommunication._uid);
        if (listUserBidsCommunication._bids.empty()) {
            listUserBidsCommunication._status = "NOK";
        } else {
            listUserBidsCommunication._status = "OK";
        }
    } catch (LoginException const &e) {
        listUserBidsCommunication._status = "NLG";
    } catch (ProtocolException const &e) {
        listUserBidsCommunication._status = "ERR";
    }
    response = listUserBidsCommunication.encodeResponse();
}

void ListAllAuctionsCommand::handle(std::stringstream &message,
                                    std::stringstream &response,
                                    Server &receiver) {
    ListAllAuctionsCommunication listAllAuctionsCommunication;
    try {
        listAllAuctionsCommunication.decodeRequest(message);
        listAllAuctionsCommunication._auctions =
            receiver._database->getAllAuctions();
        if (listAllAuctionsCommunication._auctions.empty()) {
            listAllAuctionsCommunication._status = "NOK";
        } else {
            listAllAuctionsCommunication._status = "OK";
        }
    } catch (ProtocolException const &e) {
        listAllAuctionsCommunication._status = "ERR";
    }
    response = listAllAuctionsCommunication.encodeResponse();
}

void ShowRecordCommand::handle(std::stringstream &message,
                               std::stringstream &response, Server &receiver) {
    (void)receiver;
    (void)message;
    (void)response;
}

void OpenCommand::handle(std::stringstream &message,
                         std::stringstream &response, Server &receiver) {
    OpenAuctionCommunication openAuctionCommunication;

    try {
        openAuctionCommunication.decodeRequest(message);
        std::string aid = receiver._database->createAuction(
            openAuctionCommunication._uid, openAuctionCommunication._password,
            openAuctionCommunication._name, openAuctionCommunication._startValue,
            openAuctionCommunication._timeActive,
            openAuctionCommunication._fileName,
            openAuctionCommunication._fileData);
        openAuctionCommunication._status = "OK";
        openAuctionCommunication._aid = aid;
    }
    catch (LoginException const &e) {
        openAuctionCommunication._status = "NLG";
    } catch (AidException const &e) {
        openAuctionCommunication._status = "NOK";
    } catch (ProtocolException const &e) {
        openAuctionCommunication._status = "ERR";
    }

    response = openAuctionCommunication.encodeResponse();
}

void CloseCommand::handle(std::stringstream &message,
                          std::stringstream &response, Server &receiver) {
    (void)receiver;
    (void)message;
    (void)response;
}

void ShowAssetCommand::handle(std::stringstream &message,
                              std::stringstream &response, Server &receiver) {
    (void)receiver;
    (void)message;
    (void)response;
}

void BidCommand::handle(std::stringstream &message, std::stringstream &response,
                        Server &receiver) {
    (void)receiver;
    (void)message;
    (void)response;
}