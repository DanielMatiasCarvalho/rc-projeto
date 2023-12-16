
/**
 * @file command.cpp
 * @brief Implementation of the CommandManager class and its derived classes.
 *
 * This file contains the implementation of the CommandManager class, which is
 * responsible for registering and handling commands. It also includes the
 * implementation of the derived classes which handle specific commands.
 */
#include "command.hpp"

void CommandManager::registerCommand(std::shared_ptr<CommandHandler> handler,
                                     bool isTCP) {
    if (isTCP) {  // Checks if the command is TCP or UDP
        this->_handlersTCP.insert(
            {handler->_code, handler});  // Inserts the handler for TCP command
    } else {
        this->_handlersUDP.insert(
            {handler->_code, handler});  // Inserts the handler for UDP command
    }
}

void CommandManager::readCommand(MessageSource &message,
                                 std::stringstream &response, Server &receiver,
                                 bool isTCP) {
    std::string code;
    try {
        for (size_t i = 0; i < 3;
             i++) {  // Reads the 3 digit code from the message
            char c = message.get();
            code.push_back(c);
        }
    } catch (ProtocolViolationException const &e) {
        protocolError(response);
        receiver.log(Message::ServerRequestDetails("Unknown", "ERR"));
        return;
    }

    if (isTCP) {
        auto handler = this->_handlersTCP.find(
            code);  // Finds the correct handler for the command
        if (handler ==
            this->_handlersTCP.end()) {  // If the handler is not found, the
                                         // command is not valid
            protocolError(response);
            receiver.log(Message::ServerRequestDetails("Unknown", "ERR"));
            return;
        }
        handler->second->handle(
            message, response,
            receiver);  // Executes the command on the correct handler
    } else {
        auto handler = this->_handlersUDP.find(
            code);  // Finds the correct handler for the command
        if (handler ==
            this->_handlersUDP.end()) {  // If the handler is not found, the
                                         // command is not valid
            protocolError(response);
            receiver.log(Message::ServerRequestDetails("Unknown", "ERR"));
            return;
        }
        handler->second->handle(
            message, response,
            receiver);  // Executes the command on the correct handler
    }
}

void LoginCommand::handle(MessageSource &message, std::stringstream &response,
                          Server &receiver) {

    LoginCommunication
        loginCommunication;  // Initialize the login communication object
    std::string result;
    try {
        loginCommunication.decodeRequest(message);  // Decode the request

        if (!receiver._database->loginUser(
                loginCommunication._uid,
                loginCommunication
                    ._password)) {  // Check if the user is registered
            loginCommunication._status = "OK";
            result = "Login Sucessful";
        } else {  // If the user is not registered, register the user
            loginCommunication._status = "REG";
            result = "Registration Sucessful";
        }
    } catch (
        LoginException const &e) {  // If the login fails, set the status to NOK
        loginCommunication._status = "NOK";
        result = "Login Failed";
    } catch (ProtocolException const
                 &e) {  // If the protocol is not valid, set the status to ERR
        loginCommunication._status = "ERR";
        result = "Protocol Error";
    }
    response = loginCommunication.encodeResponse();  // Encode the response
    receiver.log(Message::ServerRequestDetails(loginCommunication._uid, "Login",
                                               result));  // Display the message
}

void LogoutCommand::handle(MessageSource &message, std::stringstream &response,
                           Server &receiver) {
    LogoutCommunication
        logoutCommunication;  // Initialize the logout communication object
    std::string result;
    try {
        logoutCommunication.decodeRequest(message);  // Decode the request
        receiver._database->logoutUser(
            logoutCommunication._uid,
            logoutCommunication._password);  // Logout the user
        logoutCommunication._status =
            "OK";  // Set the status to OK if everything goes right
        result = "Logout Sucessful";
    } catch (LoginException const
                 &e) {  // If the logout fails, set the status to NOK
        logoutCommunication._status = "NOK";
        result = "Logout Failed";
    } catch (UnregisteredException const
                 &e) {  // If the user is not registered, set the status to UNR
        logoutCommunication._status = "UNR";
        result = "User Not Registered";
    } catch (ProtocolException const
                 &e) {  // If the protocol is not valid, set the status to ERR
        logoutCommunication._status = "ERR";
        result = "Protocol Error";
    }
    response = logoutCommunication.encodeResponse();  // Encode the response
    receiver.log(Message::ServerRequestDetails(logoutCommunication._uid,
                                               "Logout",
                                               result));  // Display the message
}

void UnregisterCommand::handle(MessageSource &message,
                               std::stringstream &response, Server &receiver) {
    UnregisterCommunication
        unregisterCommunication;  // Initialize the unregister communication
                                  // object
    std::string result;
    try {
        unregisterCommunication.decodeRequest(message);  // Decode the request
        receiver._database->unregisterUser(
            unregisterCommunication._uid,
            unregisterCommunication._password);  // Unregister the user
        unregisterCommunication._status =
            "OK";  // Set the status to OK if everything goes right
        result = "Unregister Sucessful";
    } catch (LoginException const
                 &e) {  // If the unregister fails, set the status to NOK
        unregisterCommunication._status = "NOK";
        result = "Unregister Failed";
    } catch (UnregisteredException const
                 &e) {  // If the user is not registered, set the status to UNR
        unregisterCommunication._status = "UNR";
        result = "User Not Registered";
    } catch (ProtocolException const
                 &e) {  // If the protocol is not valid, set the status to ERR
        unregisterCommunication._status = "ERR";
        result = "Protocol Error";
    }
    response = unregisterCommunication.encodeResponse();
    receiver.log(Message::ServerRequestDetails(unregisterCommunication._uid,
                                               "Unregister",
                                               result));  // Display the message
}

void ListUserAuctionsCommand::handle(MessageSource &message,
                                     std::stringstream &response,
                                     Server &receiver) {

    ListUserAuctionsCommunication
        listUserAuctionsCommunication;  // Initialize the list user auctions
                                        // communication object
    std::string result;
    try {
        listUserAuctionsCommunication.decodeRequest(
            message);  // Decode the request
        listUserAuctionsCommunication._auctions =
            receiver._database->getUserAuctions(
                listUserAuctionsCommunication._uid);  // Get the user auctions
        if (listUserAuctionsCommunication._auctions
                .empty()) {  // If the user has no auctions, set the status to
                             // NOK
            listUserAuctionsCommunication._status = "NOK";
            result = "No Auctions";
        } else {  // If the user has auctions, set the status to OK
            listUserAuctionsCommunication._status = "OK";
            result = "Auctions Listed";
        }
    } catch (LoginException const
                 &e) {  // If user is not logged in, set the status to NLG
        listUserAuctionsCommunication._status = "NLG";
        result = "User Not Logged In";
    } catch (ProtocolException const
                 &e) {  // If the protocol is not valid, set the status to ERR
        listUserAuctionsCommunication._status = "ERR";
        result = "Protocol Error";
    }
    response =
        listUserAuctionsCommunication.encodeResponse();  // Encode the response
    receiver.log(Message::ServerRequestDetails(
        listUserAuctionsCommunication._uid, "List User Auctions",
        result));  // Display the message
}

void ListUserBidsCommand::handle(MessageSource &message,
                                 std::stringstream &response,
                                 Server &receiver) {

    ListUserBidsCommunication
        listUserBidsCommunication;  // Initialize the list user bids
                                    // communication object
    std::string result;
    try {
        listUserBidsCommunication.decodeRequest(message);  // Decode the request
        listUserBidsCommunication._bids = receiver._database->getUserBids(
            listUserBidsCommunication._uid);  // Get the user bids
        if (listUserBidsCommunication._bids
                .empty()) {  // If the user has no bids, set the status to NOK
            listUserBidsCommunication._status = "NOK";
            result = "No Bids";
        } else {  // If the user has bids, set the status to OK
            listUserBidsCommunication._status = "OK";
            result = "Auctions Listed";
        }
    } catch (LoginException const
                 &e) {  // If user is not logged in, set the status to NLG
        listUserBidsCommunication._status = "NLG";
        result = "User Not Logged In";
    } catch (ProtocolException const
                 &e) {  // If the protocol is not valid, set the status to ERR
        listUserBidsCommunication._status = "ERR";
        result = "Protocol Error";
    }
    response =
        listUserBidsCommunication.encodeResponse();  // Encode the response
    receiver.log(Message::ServerRequestDetails(listUserBidsCommunication._uid,
                                               "List User Bids",
                                               result));  // Display the message
}

void ListAllAuctionsCommand::handle(MessageSource &message,
                                    std::stringstream &response,
                                    Server &receiver) {
    ListAllAuctionsCommunication
        listAllAuctionsCommunication;  // Initialize the list all auctions
                                       // communication object
    std::string result;
    try {
        listAllAuctionsCommunication.decodeRequest(
            message);  // Decode the request
        listAllAuctionsCommunication._auctions =
            receiver._database->getAllAuctions();  // Get all the auctions
        if (listAllAuctionsCommunication._auctions
                .empty()) {  // If there are no auctions, set the status to NOK
            listAllAuctionsCommunication._status = "NOK";
            result = "No Auctions";
        } else {  // If there are auctions, set the status to OK
            listAllAuctionsCommunication._status = "OK";
            result = "Auctions Listed";
        }
    } catch (ProtocolException const
                 &e) {  // If the protocol is not valid, set the status to ERR
        listAllAuctionsCommunication._status = "ERR";
        result = "Protocol Error";
    }
    response =
        listAllAuctionsCommunication.encodeResponse();  // Encode the response
    receiver.log(Message::ServerRequestDetails("List Auctions",
                                               result));  // Display the message
}

void ShowRecordCommand::handle(MessageSource &message,
                               std::stringstream &response, Server &receiver) {
    ShowRecordCommunication
        showRecordCommunication;  // Initialize the show record communication
                                  // object
    std::string result;
    try {
        showRecordCommunication.decodeRequest(message);  // Decode the request
        AuctionStartInfo auctionStartInfo =
            receiver._database->getAuctionStartInfo(
                showRecordCommunication._aid);  // Get the auction start info

        showRecordCommunication._hostUid =
            auctionStartInfo.uid;  // Set the host uid
        showRecordCommunication._auctionName =
            auctionStartInfo.name;  // Set the auction name
        showRecordCommunication._startValue =
            auctionStartInfo.startValue;  // Set the start value
        showRecordCommunication._timeActive =
            (int)auctionStartInfo.timeActive;  // Set the time active
        showRecordCommunication._assetFname = receiver._database->getAssetName(
            showRecordCommunication._aid);  // Set the asset filename
        showRecordCommunication._startDateTime =
            auctionStartInfo.startTime;  // Set the start date time

        std::vector<AuctionBidInfo> auctionBidInfo =
            receiver._database->getAuctionBids(
                showRecordCommunication._aid);  // Get the auction bids
        long unsigned int size = auctionBidInfo.size();
        long unsigned int i = 0;
        if (size >
            50) {  // If there are more than 50 bids, only show the last 50
            i = size - 50;
        }
        for (; i < size; i++) {  // Set the bids
            showRecordCommunication._bidderUids.push_back(
                auctionBidInfo[i].uid);  // Set the bidder uids
            showRecordCommunication._bidValues.push_back(
                auctionBidInfo[i].bidValue);  // Set the bid values
            showRecordCommunication._bidDateTime.push_back(
                auctionBidInfo[i].bidTime);  // Set the bid date time
            int bidSecTime = (int)difftime(auctionBidInfo[i].bidTime,
                                           auctionStartInfo.startTime);
            showRecordCommunication._bidSecTimes.push_back(
                bidSecTime);  // Set the bid sec times
        }

        if (receiver._database->hasAuctionEnded(
                showRecordCommunication
                    ._aid)) {  // Check if the auction has ended

            AuctionEndInfo auctionEndInfo =
                receiver._database->getAuctionEndInfo(
                    showRecordCommunication._aid);  // Get the auction end info
            showRecordCommunication._hasEnded = true;  // Set the has ended flag
            showRecordCommunication._endDateTime =
                auctionEndInfo.endTime;  // Set the end date time
            int endSecTime = (int)difftime(auctionEndInfo.endTime,
                                           auctionStartInfo.startTime);
            showRecordCommunication._endSecTime =
                endSecTime;  // Set the end sec time
        }

        showRecordCommunication._status = "OK";  // Set the status to OK
        result = "Record Shown";
    } catch (AuctionException const
                 &e) {  // If the auction does not exist, set the status to NOK
        showRecordCommunication._status = "NOK";
        result = "Auction Does Not Exist";
    } catch (ProtocolException const
                 &e) {  // If the protocol is not valid, set the status to ERR
        showRecordCommunication._status = "ERR";
        result = "Protocol Error";
    }
    response = showRecordCommunication.encodeResponse();  // Encode the response
    receiver.log(Message::ServerRequestDetails("Show Record",
                                               result));  // Display the message
}

void OpenCommand::handle(MessageSource &message, std::stringstream &response,
                         Server &receiver) {
    OpenAuctionCommunication
        openAuctionCommunication;  // Initialize the open auction communication
                                   // object
    std::string result;
    try {
        openAuctionCommunication.decodeRequest(message);  // Decode the request
        std::string aid = receiver._database->createAuction(
            openAuctionCommunication._uid, openAuctionCommunication._password,
            openAuctionCommunication._name,
            openAuctionCommunication._startValue,
            openAuctionCommunication._timeActive,
            openAuctionCommunication._fileName,
            openAuctionCommunication._fileData);  // Create the auction
        openAuctionCommunication._status =
            "OK";  // Set the status to OK if everything goes right
        result = "Auction Created";
        openAuctionCommunication._aid = aid;  // Set the auction id
    } catch (LoginException const
                 &e) {  // If the user is not logged in, set the status to NLG
        openAuctionCommunication._status = "NLG";
        result = "User Not Logged In";
    } catch (AidException const &e) {  // If the auction id is unavailable, set
                                       // the status to NOK
        openAuctionCommunication._status = "NOK";
        result = "Auction ID Unavailable";
    } catch (ProtocolException const
                 &e) {  // If the protocol is not valid, set the status to ERR
        openAuctionCommunication._status = "ERR";
        result = "Protocol Error";
    }

    response = openAuctionCommunication.encodeResponse();  // Encode the
                                                           // response
    receiver.log(Message::ServerRequestDetails(openAuctionCommunication._uid,
                                               "Open Auction",
                                               result));  // Display the message
}

void CloseCommand::handle(MessageSource &message, std::stringstream &response,
                          Server &receiver) {
    CloseAuctionCommunication
        closeAuctionCommunication;  // Initialize the close auction
                                    // communication object
    std::string result;
    try {
        closeAuctionCommunication.decodeRequest(message);  // Decode the request
        receiver._database->closeAuction(
            closeAuctionCommunication._uid, closeAuctionCommunication._password,
            closeAuctionCommunication._aid);  // Close the auction
        closeAuctionCommunication._status =
            "OK";  // Set the status to OK if everything goes right
        result = "Auction Closed";
    } catch (LoginException const
                 &e) {  // If the user is not logged in, set the status to NLG
        closeAuctionCommunication._status = "NLG";
        result = "User Not Logged In";
    } catch (AuctionException const
                 &e) {  // If the auction does not exist, set the status to NOK
        closeAuctionCommunication._status = "EAU";
        result = "Auction Does Not Exist";
    } catch (AuctionEndedException const &e) {  // If the auction has already
                                                // ended, set the status to END
        closeAuctionCommunication._status = "END";
        result = "Auction Already Ended";
    } catch (
        AuctionOwnerException const &
            e) {  // If the user is not the auction owner, set the status to EOW
        closeAuctionCommunication._status = "EOW";
        result = "User Not Auction Owner";
    } catch (ProtocolException const
                 &e) {  // If the protocol is not valid, set the status to ERR
        closeAuctionCommunication._status = "ERR";
        result = "Protocol Error";
    }
    response =
        closeAuctionCommunication.encodeResponse();  // Encode the response
    receiver.log(Message::ServerRequestDetails(closeAuctionCommunication._uid,
                                               "Close Auction",
                                               result));  // Display the message
}

void ShowAssetCommand::handle(MessageSource &message,
                              std::stringstream &response, Server &receiver) {
    ShowAssetCommunication showAssetCommunication;  // Initialize the show asset
                                                    // communication object
    std::string result;
    try {
        showAssetCommunication.decodeRequest(message);  // Decode the request
        showAssetCommunication._fileSize = receiver._database->getAuctionAsset(
            showAssetCommunication._aid, showAssetCommunication._fileName,
            showAssetCommunication._fileData);  // Get the auction asset
        showAssetCommunication._status =
            "OK";  // Set the status to OK if everything goes right
        result = "Asset Shown";
    } catch (
        AuctionException const &e) {  // If there is a problem with the auction
                                      // or the file, set the status to NOK
        showAssetCommunication._status = "NOK";
        result = "Problem With Auction Or File";
    } catch (ProtocolException const
                 &e) {  // If the protocol is not valid, set the status to ERR
        showAssetCommunication._status = "ERR";
        result = "Protocol Error";
    }
    response = showAssetCommunication.encodeResponse();  // Encode the response
    receiver.log(Message::ServerRequestDetails("Show Asset",
                                               result));  // Display the message
}

void BidCommand::handle(MessageSource &message, std::stringstream &response,
                        Server &receiver) {
    BidCommunication
        bidCommunication;  // Initialize the bid communication object
    std::string result;
    try {
        bidCommunication.decodeRequest(message);  // Decode the request
        receiver._database->bidAuction(
            bidCommunication._uid, bidCommunication._password,
            bidCommunication._aid, bidCommunication._value);  // Bid the auction
        bidCommunication._status =
            "ACC";  // Set the status to ACC if everything goes right
        result = "Bid Accepted";
    } catch (LoginException const
                 &e) {  // If the user is not logged in, set the status to NLG
        bidCommunication._status = "NLG";
        result = "User Not Logged In";
    } catch (AuctionException const
                 &e) {  // If the auction does not exist, set the status to NOK
        bidCommunication._status = "NOK";
        result = "Auction Does Not Exist";
    } catch (AuctionEndedException const &e) {  // If the auction has already
                                                // ended, set the status to NOK
        bidCommunication._status = "NOK";
        result = "Auction Already Ended";
    } catch (BidValueException const
                 &e) {  // If the bid value is too low, set the status to REF
        bidCommunication._status = "REF";
        result = "Bid Value Too Low";
    } catch (AuctionOwnerException const &e) {  // If the user is the auction
                                                // owner, set the status to ILG
        bidCommunication._status = "ILG";
        result = "User Is Auction Owner";
    } catch (ProtocolException const
                 &e) {  // If the protocol is not valid, set the status to ERR
        bidCommunication._status = "ERR";
        result = "Protocol Error";
    }
    response = bidCommunication.encodeResponse();  // Encode the response
    receiver.log(Message::ServerRequestDetails(bidCommunication._uid, "Bid",
                                               result));  // Display the message
}

void protocolError(std::stringstream &response) {
    std::string str =
        PROTOCOL_ERROR_IDENTIFIER;  // The protocol error identifier
    for (auto c :
         str) {  // Writes the protocol error identifier to the response stream
        response.put(c);
    }
    response.put('\n');  // Writes a newline character to the response stream
}