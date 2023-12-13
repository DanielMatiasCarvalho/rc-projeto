
/**
 * @file command.cpp
 * @brief Implementation of the CommandManager class and its derived classes.
 *
 * This file contains the implementation of the CommandManager class, which is responsible for registering and handling commands.
 * It also includes the implementation of the derived classes which handle specific commands.
 */
#include "command.hpp"

/**
 * Registers a command handler.
 *
 * @param handler The command handler to register.
 */
void CommandManager::registerCommand(std::shared_ptr<CommandHandler> handler,
                                     bool isTCP) {
    if (isTCP) {  //Checks if the command is TCP or UDP
        this->_handlersTCP.insert(
            {handler->_code, handler});  //Inserts the handler for TCP command
    } else {
        this->_handlersUDP.insert(
            {handler->_code, handler});  //Inserts the handler for UDP command
    }
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
                                 std::stringstream &response, Server &receiver,
                                 bool isTCP) {
    std::string code;
    for (size_t i = 0; i < 3; i++) {  //Reads the 3 digit code from the message
        char c = (char)message.get();
        code.push_back(c);
    }

    if (isTCP) {
        auto handler = this->_handlersTCP.find(
            code);  //Finds the correct handler for the command
        if (handler ==
            this->_handlersTCP
                .end()) {  //If the handler is not found, the command is not valid
            protocolError(response);
            return;
        }
        handler->second->handle(
            message, response,
            receiver);  //Executes the command on the correct handler
    } else {
        auto handler = this->_handlersUDP.find(
            code);  //Finds the correct handler for the command
        if (handler ==
            this->_handlersUDP
                .end()) {  //If the handler is not found, the command is not valid
            protocolError(response);
            return;
        }
        handler->second->handle(
            message, response,
            receiver);  //Executes the command on the correct handler
    }
}

/**
 * @brief Handles the login command.
 *
 * @param message The MessageSource object containing the command message.
 * @param response The std::stringstream object to store the response.
 * @param receiver The Server object to interact with.
 */
void LoginCommand::handle(MessageSource &message, std::stringstream &response,
                          Server &receiver) {

    LoginCommunication
        loginCommunication;  //Initialize the login communication object
    try {
        loginCommunication.decodeRequest(message);  //Decode the request

        if (!receiver._database->loginUser(
                loginCommunication._uid,
                loginCommunication
                    ._password)) {  //Check if the user is registered
            loginCommunication._status = "OK";
        } else {  //If the user is not registered, register the user
            loginCommunication._status = "REG";
        }
    } catch (
        LoginException const &e) {  //If the login fails, set the status to NOK
        loginCommunication._status = "NOK";
    } catch (ProtocolException const
                 &e) {  //If the protocol is not valid, set the status to ERR
        loginCommunication._status = "ERR";
    }
    response = loginCommunication.encodeResponse();  //Encode the response
    receiver.showMessage(Message::ServerRequestDetails(
        loginCommunication._uid, "Login",
        loginCommunication._status));  //Display the message
}

/**
 * @brief Handles the logout command.
 *
 * @param message The MessageSource object containing the command message.
 * @param response The std::stringstream object to store the response.
 * @param receiver The Server object to interact with.
 */
void LogoutCommand::handle(MessageSource &message, std::stringstream &response,
                           Server &receiver) {
    LogoutCommunication
        logoutCommunication;  //Initialize the logout communication object
    try {
        logoutCommunication.decodeRequest(message);  //Decode the request
        receiver._database->logoutUser(
            logoutCommunication._uid,
            logoutCommunication._password);  //Logout the user
        logoutCommunication._status =
            "OK";  //Set the status to OK if everything goes right
    } catch (
        LoginException const &e) {  //If the logout fails, set the status to NOK
        logoutCommunication._status = "NOK";
    } catch (UnregisteredException const
                 &e) {  //If the user is not registered, set the status to UNR
        logoutCommunication._status = "UNR";
    } catch (ProtocolException const
                 &e) {  //If the protocol is not valid, set the status to ERR
        logoutCommunication._status = "ERR";
    }
    response = logoutCommunication.encodeResponse();  //Encode the response
    receiver.showMessage(Message::ServerRequestDetails(
        logoutCommunication._uid, "Logout",
        logoutCommunication._status));  //Display the message
}

/**
 * @brief Handles the unregister command.
 *
 * @param message The MessageSource object containing the command message.
 * @param response The std::stringstream object to store the response.
 * @param receiver The Server object to interact with.
 */
void UnregisterCommand::handle(MessageSource &message,
                               std::stringstream &response, Server &receiver) {
    UnregisterCommunication
        unregisterCommunication;  //Initialize the unregister communication object
    try {
        unregisterCommunication.decodeRequest(message);  //Decode the request
        receiver._database->unregisterUser(
            unregisterCommunication._uid,
            unregisterCommunication._password);  //Unregister the user
        unregisterCommunication._status =
            "OK";  //Set the status to OK if everything goes right
    } catch (
        LoginException const &e) {  //If the login fails, set the status to NOK
        unregisterCommunication._status = "NOK";
    } catch (UnregisteredException const
                 &e) {  //If the user is not registered, set the status to UNR
        unregisterCommunication._status = "UNR";
    } catch (ProtocolException const
                 &e) {  //If the protocol is not valid, set the status to ERR
        unregisterCommunication._status = "ERR";
    }
    response = unregisterCommunication.encodeResponse();
    receiver.showMessage(Message::ServerRequestDetails(
        unregisterCommunication._uid, "Unregister",
        unregisterCommunication._status));
}

/**
 * @brief Handles the list user auctions command.
 *
 * @param message The MessageSource object containing the command message.
 * @param response The std::stringstream object to store the response.
 * @param receiver The Server object to interact with.
 */
void ListUserAuctionsCommand::handle(MessageSource &message,
                                     std::stringstream &response,
                                     Server &receiver) {

    ListUserAuctionsCommunication
        listUserAuctionsCommunication;  //Initialize the list user auctions communication object
    try {
        listUserAuctionsCommunication.decodeRequest(
            message);  //Decode the request
        listUserAuctionsCommunication._auctions =
            receiver._database->getUserAuctions(
                listUserAuctionsCommunication._uid);  //Get the user auctions
        if (listUserAuctionsCommunication._auctions
                .empty()) {  //If the user has no auctions, set the status to NOK
            listUserAuctionsCommunication._status = "NOK";
        } else {  //If the user has auctions, set the status to OK
            listUserAuctionsCommunication._status = "OK";
        }
    } catch (LoginException const
                 &e) {  //If user is not logged in, set the status to NLG
        listUserAuctionsCommunication._status = "NLG";
    } catch (ProtocolException const
                 &e) {  //If the protocol is not valid, set the status to ERR
        listUserAuctionsCommunication._status = "ERR";
    }
    response =
        listUserAuctionsCommunication.encodeResponse();  //Encode the response
    receiver.showMessage(Message::ServerRequestDetails(
        listUserAuctionsCommunication._uid, "List User Auctions",
        listUserAuctionsCommunication._status));  //Display the message
}

/**
 * @brief Handles the list user bids command.
 *
 * @param message The MessageSource object containing the command message.
 * @param response The std::stringstream object to store the response.
 * @param receiver The Server object to interact with.
 */
void ListUserBidsCommand::handle(MessageSource &message,
                                 std::stringstream &response,
                                 Server &receiver) {

    ListUserBidsCommunication
        listUserBidsCommunication;  //Initialize the list user bids communication object
    try {
        listUserBidsCommunication.decodeRequest(message);  //Decode the request
        listUserBidsCommunication._bids = receiver._database->getUserBids(
            listUserBidsCommunication._uid);  //Get the user bids
        if (listUserBidsCommunication._bids
                .empty()) {  //If the user has no bids, set the status to NOK
            listUserBidsCommunication._status = "NOK";
        } else {  //If the user has bids, set the status to OK
            listUserBidsCommunication._status = "OK";
        }
    } catch (LoginException const
                 &e) {  //If user is not logged in, set the status to NLG
        listUserBidsCommunication._status = "NLG";
    } catch (ProtocolException const
                 &e) {  //If the protocol is not valid, set the status to ERR
        listUserBidsCommunication._status = "ERR";
    }
    response =
        listUserBidsCommunication.encodeResponse();  //Encode the response
    receiver.showMessage(Message::ServerRequestDetails(
        listUserBidsCommunication._uid, "List User Bids",
        listUserBidsCommunication._status));  //Display the message
}

/**
 * @brief Handles the list all auctions command.
 *
 * @param message The MessageSource object containing the command message.
 * @param response The std::stringstream object to store the response.
 * @param receiver The Server object to interact with.
 */
void ListAllAuctionsCommand::handle(MessageSource &message,
                                    std::stringstream &response,
                                    Server &receiver) {
    ListAllAuctionsCommunication
        listAllAuctionsCommunication;  //Initialize the list all auctions communication object
    try {
        listAllAuctionsCommunication.decodeRequest(
            message);  //Decode the request
        listAllAuctionsCommunication._auctions =
            receiver._database->getAllAuctions();  //Get all the auctions
        if (listAllAuctionsCommunication._auctions
                .empty()) {  //If there are no auctions, set the status to NOK
            listAllAuctionsCommunication._status = "NOK";
        } else {  //If there are auctions, set the status to OK
            listAllAuctionsCommunication._status = "OK";
        }
    } catch (ProtocolException const
                 &e) {  //If the protocol is not valid, set the status to ERR
        listAllAuctionsCommunication._status = "ERR";
    }
    response =
        listAllAuctionsCommunication.encodeResponse();  //Encode the response
    receiver.showMessage(Message::ServerRequestDetails(
        "List Auctions",
        listAllAuctionsCommunication._status));  //Display the message
}

/**
 * @brief Handles the show record command.
 *
 * @param message The MessageSource object containing the command message.
 * @param response The std::stringstream object to store the response.
 * @param receiver The Server object to interact with.
 */
void ShowRecordCommand::handle(MessageSource &message,
                               std::stringstream &response, Server &receiver) {
    ShowRecordCommunication
        showRecordCommunication;  //Initialize the show record communication object
    try {
        showRecordCommunication.decodeRequest(message);  //Decode the request
        AuctionStartInfo auctionStartInfo =
            receiver._database->getAuctionStartInfo(
                showRecordCommunication._aid);  //Get the auction start info

        showRecordCommunication._hostUid =
            auctionStartInfo.uid;  //Set the host uid
        showRecordCommunication._auctionName =
            auctionStartInfo.name;  //Set the auction name
        showRecordCommunication._startValue =
            auctionStartInfo.startValue;  //Set the start value
        showRecordCommunication._timeActive =
            (int)auctionStartInfo.timeActive;  //Set the time active
        showRecordCommunication._assetFname = receiver._database->getAssetName(
            showRecordCommunication._aid);  //Set the asset filename
        showRecordCommunication._startDateTime =
            auctionStartInfo.startTime;  //Set the start date time

        std::vector<AuctionBidInfo> auctionBidInfo =
            receiver._database->getAuctionBids(
                showRecordCommunication._aid);  //Get the auction bids
        long unsigned int size = auctionBidInfo.size();
        long unsigned int i = 0;
        if (size >
            50) {  //If there are more than 50 bids, only show the last 50
            i = size - 50;
        }
        for (; i < size; i++) {  //Set the bids
            showRecordCommunication._bidderUids.push_back(
                auctionBidInfo[i].uid);  //Set the bidder uids
            showRecordCommunication._bidValues.push_back(
                auctionBidInfo[i].bidValue);  //Set the bid values
            showRecordCommunication._bidDateTime.push_back(
                auctionBidInfo[i].bidTime);  //Set the bid date time
            int bidSecTime = (int)difftime(auctionBidInfo[i].bidTime,
                                           auctionStartInfo.startTime);
            showRecordCommunication._bidSecTimes.push_back(
                bidSecTime);  //Set the bid sec times
        }

        if (receiver._database->hasAuctionEnded(
                showRecordCommunication
                    ._aid)) {  //Check if the auction has ended

            AuctionEndInfo auctionEndInfo =
                receiver._database->getAuctionEndInfo(
                    showRecordCommunication._aid);  //Get the auction end info
            showRecordCommunication._hasEnded = true;  //Set the has ended flag
            showRecordCommunication._endDateTime =
                auctionEndInfo.endTime;  //Set the end date time
            int endSecTime = (int)difftime(auctionEndInfo.endTime,
                                           auctionStartInfo.startTime);
            showRecordCommunication._endSecTime =
                endSecTime;  //Set the end sec time
        }

        showRecordCommunication._status = "OK";  //Set the status to OK
    } catch (AuctionException const
                 &e) {  //If the auction does not exist, set the status to NOK
        showRecordCommunication._status = "NOK";
    } catch (ProtocolException const
                 &e) {  //If the protocol is not valid, set the status to ERR
        showRecordCommunication._status = "ERR";
    }
    response = showRecordCommunication.encodeResponse();  //Encode the response
    receiver.showMessage(Message::ServerRequestDetails(
        "Show Record", showRecordCommunication._status));  //Display the message
}

/**
 * @brief Handles the open command.
 *
 * @param message The MessageSource object containing the command message.
 * @param response The std::stringstream object to store the response.
 * @param receiver The Server object to interact with.
 */
void OpenCommand::handle(MessageSource &message, std::stringstream &response,
                         Server &receiver) {
    OpenAuctionCommunication
        openAuctionCommunication;  //Initialize the open auction communication object

    try {
        openAuctionCommunication.decodeRequest(message);  //Decode the request
        std::string aid = receiver._database->createAuction(
            openAuctionCommunication._uid, openAuctionCommunication._password,
            openAuctionCommunication._name,
            openAuctionCommunication._startValue,
            openAuctionCommunication._timeActive,
            openAuctionCommunication._fileName,
            openAuctionCommunication._fileData);  //Create the auction
        openAuctionCommunication._status =
            "OK";  //Set the status to OK if everything goes right
        openAuctionCommunication._aid = aid;  //Set the auction id
    } catch (LoginException const
                 &e) {  //If the user is not logged in, set the status to NLG
        openAuctionCommunication._status = "NLG";
    } catch (AidException const &
                 e) {  //If the auction id is unavailable, set the status to NOK
        openAuctionCommunication._status = "NOK";
    } catch (ProtocolException const
                 &e) {  //If the protocol is not valid, set the status to ERR
        openAuctionCommunication._status = "ERR";
    }

    response = openAuctionCommunication.encodeResponse();  //Encode the response
    receiver.showMessage(Message::ServerRequestDetails(
        openAuctionCommunication._uid, "Open Auction",
        openAuctionCommunication._status));  //Display the message
}

/**
 * @brief Handles the close command.
 * 
 * @param message The MessageSource object containing the command message.
 * @param response The std::stringstream object to store the response.
 * @param receiver The Server object to interact with.
 */
void CloseCommand::handle(MessageSource &message, std::stringstream &response,
                          Server &receiver) {
    CloseAuctionCommunication
        closeAuctionCommunication;  //Initialize the close auction communication object
    try {
        closeAuctionCommunication.decodeRequest(message);  //Decode the request
        receiver._database->closeAuction(
            closeAuctionCommunication._uid, closeAuctionCommunication._password,
            closeAuctionCommunication._aid);  //Close the auction
        closeAuctionCommunication._status =
            "OK";  //Set the status to OK if everything goes right
    } catch (LoginException const
                 &e) {  //If the user is not logged in, set the status to NLG
        closeAuctionCommunication._status = "NLG";
    } catch (AuctionException const
                 &e) {  //If the auction does not exist, set the status to NOK
        closeAuctionCommunication._status = "EAU";
    } catch (AuctionEndedException const &
                 e) {  //If the auction has already ended, set the status to END
        closeAuctionCommunication._status = "END";
    } catch (
        AuctionOwnerException const
            &e) {  //If the user is not the auction owner, set the status to EOW
        closeAuctionCommunication._status = "EOW";
    } catch (ProtocolException const
                 &e) {  //If the protocol is not valid, set the status to ERR
        closeAuctionCommunication._status = "ERR";
    }
    response =
        closeAuctionCommunication.encodeResponse();  //Encode the response
    receiver.showMessage(Message::ServerRequestDetails(
        closeAuctionCommunication._uid, "Close Auction",
        closeAuctionCommunication._status));  //Display the message
}

/**
 * @brief Handles the show asset command.
 *
 * @param message The MessageSource object containing the command message.
 * @param response The std::stringstream object to store the response.
 * @param receiver The Server object to interact with.
 */
void ShowAssetCommand::handle(MessageSource &message,
                              std::stringstream &response, Server &receiver) {
    ShowAssetCommunication
        showAssetCommunication;  //Initialize the show asset communication object
    try {
        showAssetCommunication.decodeRequest(message);  //Decode the request
        showAssetCommunication._fileSize = receiver._database->getAuctionAsset(
            showAssetCommunication._aid, showAssetCommunication._fileName,
            showAssetCommunication._fileData);  //Get the auction asset
        showAssetCommunication._status =
            "OK";  //Set the status to OK if everything goes right
    } catch (
        AuctionException const &
            e) {  //If there is a problem with the auction or the file, set the status to NOK
        showAssetCommunication._status = "NOK";
    } catch (ProtocolException const
                 &e) {  //If the protocol is not valid, set the status to ERR
        showAssetCommunication._status = "ERR";
    }
    response = showAssetCommunication.encodeResponse();  //Encode the response
    receiver.showMessage(Message::ServerRequestDetails(
        "Show Asset", showAssetCommunication._status));  //Display the message
}

/**
 * @brief Handles the bid command.
 *
 * @param message The MessageSource object containing the command message.
 * @param response The std::stringstream object to store the response.
 * @param receiver The Server object to interact with.
 */
void BidCommand::handle(MessageSource &message, std::stringstream &response,
                        Server &receiver) {
    BidCommunication
        bidCommunication;  //Initialize the bid communication object
    try {
        bidCommunication.decodeRequest(message);  //Decode the request
        receiver._database->bidAuction(
            bidCommunication._uid, bidCommunication._password,
            bidCommunication._aid, bidCommunication._value);  //Bid the auction
        bidCommunication._status =
            "ACC";  //Set the status to ACC if everything goes right
    } catch (LoginException const
                 &e) {  //If the user is not logged in, set the status to NLG
        bidCommunication._status = "NLG";
    } catch (AuctionException const
                 &e) {  //If the auction does not exist, set the status to NOK
        bidCommunication._status = "NOK";
    } catch (AuctionEndedException const &
                 e) {  //If the auction has already ended, set the status to NOK
        bidCommunication._status = "NOK";
    } catch (BidValueException const
                 &e) {  //If the bid value is too low, set the status to REF
        bidCommunication._status = "REF";
    } catch (AuctionOwnerException const &
                 e) {  //If the user is the auction owner, set the status to ILG
        bidCommunication._status = "ILG";
    } catch (ProtocolException const
                 &e) {  //If the protocol is not valid, set the status to ERR
        bidCommunication._status = "ERR";
    }
    response = bidCommunication.encodeResponse();  //Encode the response
    receiver.showMessage(Message::ServerRequestDetails(
        bidCommunication._uid, "Bid",
        bidCommunication._status));  //Display the message
}

/**
 * @brief Handles a protocol error.
 * 
 * This function is responsible for handling a protocol error and updating the response accordingly.
 * 
 * @param response The response stream to be updated.
 */
void protocolError(std::stringstream &response) {
    std::string str =
        PROTOCOL_ERROR_IDENTIFIER;  //The protocol error identifier
    for (auto c :
         str) {  //Writes the protocol error identifier to the response stream
        response.put(c);
    }
    response.put('\n');  //Writes a newline character to the response stream
}