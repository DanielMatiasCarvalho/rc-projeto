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
void CommandManager::readCommand(MessageSource &message,
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

void LoginCommand::handle(MessageSource &message, std::stringstream &response,
                          Server &receiver) {

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

void LogoutCommand::handle(MessageSource &message, std::stringstream &response,
                           Server &receiver) {
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

void UnregisterCommand::handle(MessageSource &message,
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

void ListUserAuctionsCommand::handle(MessageSource &message,
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

void ListUserBidsCommand::handle(MessageSource &message,
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

void ListAllAuctionsCommand::handle(MessageSource &message,
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

void ShowRecordCommand::handle(MessageSource &message,
                               std::stringstream &response, Server &receiver) {
    ShowRecordCommunication showRecordCommunication;
    try {
        showRecordCommunication.decodeRequest(message);
        AuctionStartInfo auctionStartInfo =
            receiver._database->getAuctionStartInfo(
                showRecordCommunication._aid);
        
        showRecordCommunication._hostUid = auctionStartInfo.uid;
        showRecordCommunication._auctionName = auctionStartInfo.name;
        showRecordCommunication._startValue = auctionStartInfo.startValue;
        showRecordCommunication._timeActive = (int) auctionStartInfo.timeActive;
        //showRecordCommunication._assetFName = auctionStartInfo.fileName;
        showRecordCommunication._startDateTime = auctionStartInfo.startTime;

        std::vector<AuctionBidInfo> auctionBidInfo =
            receiver._database->getAuctionBids(
                showRecordCommunication._aid);
        
        for (auto c: auctionBidInfo){
            showRecordCommunication._bidderUids.push_back(c.uid);
            showRecordCommunication._bidValues.push_back(c.bidValue);
            showRecordCommunication._bidDateTime.push_back(c.bidTime);
            int bidSecTime = (int) difftime(c.bidTime, auctionStartInfo.startTime);
            showRecordCommunication._bidSecTimes.push_back(bidSecTime);
        }
        AuctionEndInfo auctionEndInfo =
            receiver._database->getAuctionEndInfo(
                showRecordCommunication._aid);
        showRecordCommunication._hasEnded = true;
        showRecordCommunication._endDateTime = auctionEndInfo.endTime;
        int endSecTime = (int) difftime(auctionEndInfo.endTime, auctionStartInfo.startTime);
        showRecordCommunication._endSecTime = endSecTime;

        showRecordCommunication._status = "OK";
    } catch (AuctionException const &e) {
        showRecordCommunication._status = "NOK";
    } catch (AuctionEndedException const &e) {
        //Nothing is done here, because the auction has not ended
    } catch (ProtocolException const &e) {
        showRecordCommunication._status = "ERR";
    }
    response = showRecordCommunication.encodeResponse();
}

void OpenCommand::handle(MessageSource &message, std::stringstream &response,
                         Server &receiver) {
    OpenAuctionCommunication openAuctionCommunication;

    try {
        openAuctionCommunication.decodeRequest(message);
        std::string aid = receiver._database->createAuction(
            openAuctionCommunication._uid, openAuctionCommunication._password,
            openAuctionCommunication._name,
            openAuctionCommunication._startValue,
            openAuctionCommunication._timeActive,
            openAuctionCommunication._fileName,
            openAuctionCommunication._fileData);
        openAuctionCommunication._status = "OK";
        openAuctionCommunication._aid = aid;
    } catch (LoginException const &e) {
        openAuctionCommunication._status = "NLG";
    } catch (AidException const &e) {
        openAuctionCommunication._status = "NOK";
    } catch (ProtocolException const &e) {
        openAuctionCommunication._status = "ERR";
    }

    response = openAuctionCommunication.encodeResponse();
}

void CloseCommand::handle(MessageSource &message, std::stringstream &response,
                          Server &receiver) {
    CloseAuctionCommunication closeAuctionCommunication;
    try { //NOK STATUS PARA PASSE ERRADA
        closeAuctionCommunication.decodeRequest(message);
        receiver._database->closeAuction(closeAuctionCommunication._uid,
                                          closeAuctionCommunication._password,
                                          closeAuctionCommunication._aid);
        closeAuctionCommunication._status = "OK";
    } catch (LoginException const &e) {
        closeAuctionCommunication._status = "NLG";
    } catch (AuctionException const &e) {
        closeAuctionCommunication._status = "NOK";
    } catch (AuctionEndedException const &e) {
        closeAuctionCommunication._status = "END";
    } catch (AuctionOwnerException const &e) {
        closeAuctionCommunication._status = "EOW";
    } catch (ProtocolException const &e) {
        closeAuctionCommunication._status = "ERR";
    }
    response = closeAuctionCommunication.encodeResponse();
}

void ShowAssetCommand::handle(MessageSource &message,
                              std::stringstream &response, Server &receiver) {
    ShowAssetCommunication showAssetCommunication;
    try {
        showAssetCommunication.decodeRequest(message);
        showAssetCommunication._fileSize =
            receiver._database->getAuctionAsset(showAssetCommunication._aid,
                                                showAssetCommunication._fileName,
                                                showAssetCommunication._fileData);
        showAssetCommunication._status = "OK";
    } catch (AuctionException const &e) {
        showAssetCommunication._status = "NOK";
    } catch (ProtocolException const &e) {
        showAssetCommunication._status = "ERR";
    }
    response = showAssetCommunication.encodeResponse();
}

void BidCommand::handle(MessageSource &message, std::stringstream &response,
                        Server &receiver) {
    BidCommunication bidCommunication;
    try {
        bidCommunication.decodeRequest(message);
        receiver._database->bidAuction(bidCommunication._uid,
                                       bidCommunication._password,
                                       bidCommunication._aid,
                                       bidCommunication._value);
        bidCommunication._status = "ACC";
    } catch (LoginException const &e) {
        bidCommunication._status = "NLG";
    } catch (AuctionException const &e) {
        bidCommunication._status = "NOK";
    } catch (AuctionEndedException const &e) {
        bidCommunication._status = "NOK";
    } catch (BidValueException const &e) {
        bidCommunication._status = "REF";
    } catch (AuctionOwnerException const &e) {
        bidCommunication._status = "ILG";
    } catch (ProtocolException const &e) {
        bidCommunication._status = "ERR";
    }
    response = bidCommunication.encodeResponse();
}