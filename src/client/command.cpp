/**
 * @brief This file contains the implementation of various command classes used in the client application.
 * The CommandManager class is responsible for registering and executing commands.
 * Each command class implements the handle() function to perform the specific command logic.
 * The command classes handle different actions such as login, logout, auction management, bidding, and listing auctions.
 * It performs the necessary validations and communicates with the server through the client object to execute the command.
 * If any validation fails or an error occurs during command execution, appropriate exception messages are thrown.
 */
#include "command.hpp"

void CommandManager::registerCommand(std::shared_ptr<CommandHandler> handler) {
    //Insert the handler into the map
    this->handlers.insert({handler->_name, handler});

    for (auto alias : handler->_alias) {
        //Insert the handler into the map for each alias
        this->handlers.insert({alias, handler});
    }
}

void CommandManager::readCommand(std::string command, Client &receiver) {
    if (command.length() == 0) {
        //If the command is empty, return
        return;
    }

    std::string name;
    std::vector<std::string> args;

    auto position = command.find(" ");  //Find the first space in the command

    if (position == std::string::npos) {
        name = command;  //If there is no space, the command is the name
        command = "";    //The command is now empty
    } else {
        name = command.substr(
            0, position);  //The name is the string before the first space
        command.erase(0, position + 1);  //Remove the name from the string
    }

    position = command.find(" ");  //Find the next space in the command
    while (position !=
           std::string::npos) {  //While there is a space in the command
        args.push_back(command.substr(
            0, position));  //Add the string before the space to the arguments
        command.erase(
            0, position +
                   1);  //Remove the string before the space from the command
        position = command.find(" ");  //Find the next space in the command
    }

    if (command.length() != 0) {
        args.push_back(command);  //Add the last string to the arguments
    }

    auto handler =
        this->handlers.find(name);  //Find the handler for the command
    if (handler ==
        this->handlers
            .end()) {  //If the handler does not exist, throw an exception
        throw UnknownCommandException();
    }

    handler->second->handle(
        args, receiver);  //Call the handle function of the handler
}

void LoginCommand::handle(std::vector<std::string> args, Client &receiver) {
    if (args.size() != 2) {
        //If the number of arguments is not 2, throw an exception
        throw CommandArgumentException(_usage);
    }

    //Get the username and password from the arguments
    std::string UID = args[0];
    std::string password = args[1];

    if (UID.length() != 6 || password.length() != 8 || !isNumeric(UID)) {
        //If the username or password is invalid, throw an exception
        throw CommandArgumentException(_usage);
    }

    if (receiver._user
            .isLoggedIn()) {  //If the user is already logged in, show a message
        Message::UserAlreadyLoggedIn();
        return;
    }

    //Create a LoginCommunication object and set the username and password
    LoginCommunication loginCommunication;
    loginCommunication._uid = UID;
    loginCommunication._password = password;

    receiver.processRequest(
        loginCommunication);  //Send the request to the server, receiving its response

    if (loginCommunication._status == "OK") {
        //If the response is OK, show a message and login the user
        Message::UserLoginSuccess();
        receiver._user.logIn(UID, password);
    } else if (loginCommunication._status == "NOK") {
        //If the response is NOK, show a message for incorrect match (username and/or password)
        Message::WrongPassword();
    } else if (loginCommunication._status == "REG") {
        //If the response is REG, show a register sucess message and login the user
        Message::UserRegisterSuccess();
        receiver._user.logIn(UID, password);
    }
}

void LogoutCommand::handle(std::vector<std::string> args, Client &receiver) {
    if (args.size() != 0) {
        //If the number of arguments is not 0, throw an exception
        throw CommandArgumentException(_usage);
    }

    if (!receiver._user
             .isLoggedIn()) {  //If the user is not logged in, show a message
        Message::UserNotLoggedIn();
        return;
    }

    //Create a LogoutCommunication object and set the username and password
    LogoutCommunication logoutCommunication;
    logoutCommunication._uid = receiver._user.getUsername();
    logoutCommunication._password = receiver._user.getPassword();

    receiver.processRequest(
        logoutCommunication);  //Send the request to the server, receiving its response

    if (logoutCommunication._status == "OK") {
        //If the response is OK, show a message and logout the user
        Message::UserLogoutSucess();
        receiver._user.logOut();
    } else if (logoutCommunication._status == "NOK") {
        //If the response is NOK, show a message
        Message::UserNotLoggedIn();
    } else if (logoutCommunication._status == "UNR") {
        //If the response is UNR, show a message
        Message::UserNotRegistered();
    }
}

void UnregisterCommand::handle(std::vector<std::string> args,
                               Client &receiver) {
    if (args.size() != 0) {
        //If the number of arguments is not 0, throw an exception
        throw CommandArgumentException(_usage);
    }

    if (!receiver._user
             .isLoggedIn()) {  //If the user is not logged in, show a message
        Message::UserNotLoggedIn();
        return;
    }

    //Create an UnregisterCommunication object and set the username and password
    UnregisterCommunication unregisterCommunication;
    unregisterCommunication._uid = receiver._user.getUsername();
    unregisterCommunication._password = receiver._user.getPassword();

    receiver.processRequest(
        unregisterCommunication);  //Send the request to the server, receiving its response

    if (unregisterCommunication._status == "OK") {
        //If the response is OK, show a message and logout the user
        Message::UserUnregisterSucess();
        receiver._user.logOut();
    } else if (unregisterCommunication._status == "NOK") {
        //If the response is NOK, show a message
        Message::UserNotLoggedIn();
    } else if (unregisterCommunication._status == "UNR") {
        //If the response is UNR, show a message
        Message::UserNotRegistered();
    }
}

void ExitCommand::handle(std::vector<std::string> args, Client &receiver) {
    if (args.size() != 0) {
        //If the number of arguments is not 0, throw an exception
        throw CommandArgumentException(_usage);
    }

    if (receiver._user.isLoggedIn()) {
        //If the user is logged in, show a message and return
        Message::UserIsLoggedIn();
        return;
    }

    receiver._toExit = true;  //Set the exit flag to true
}

void OpenCommand::handle(std::vector<std::string> args, Client &receiver) {
    if (args.size() != 4) {
        //If the number of arguments is not 4, throw an exception
        throw CommandArgumentException(_usage);
    }

    //Get the name, asset filename, start value, and time active from the arguments
    std::string name = args[0];
    std::string asset_fname = args[1];
    std::string start_value = args[2];
    std::string timeactive = args[3];

    if (name.length() > 10 || asset_fname.length() > 24 ||
        start_value.length() > 6 || timeactive.length() > 5) {
        //If any of the arguments is too big, throw an exception
        throw CommandArgumentException(_usage);
    }

    if (!isNumeric(start_value) || !isNumeric(timeactive)) {
        //If the start value or time active is not numeric, throw an exception
        throw CommandArgumentException(_usage);
    }

    if (!receiver._user.isLoggedIn()) {
        //If the user is not logged in, show a message and return
        Message::UserNotLoggedIn();
        return;
    }

    /**Create an OpenAuctionCommunication object and set the username, password, 
    name, start value, time active, asset filename, file size, and file data*/
    OpenAuctionCommunication comm;

    comm._uid = receiver._user.getUsername();
    comm._password = receiver._user.getPassword();
    comm._name = name;
    comm._startValue = atoi(start_value.c_str());
    comm._timeActive = atoi(timeactive.c_str());
    comm._fileName = asset_fname;
    comm._fileSize = receiver.getFileSize(asset_fname);
    comm._fileData = receiver.readFile(asset_fname);

    if (comm._fileSize > PROTOCOL_MAX_FILE_SIZE) {
        //If the file size is too big, show a message and return
        Message::FileTooBig();
        return;
    }

    receiver.processRequest(
        comm);  //Send the request to the server, receiving its response

    if (comm._status == "OK") {
        //If the response is OK, show a message
        Message::AuctionCreated(comm._aid);
    } else if (comm._status == "NLG") {
        //If the response is NLG, show a message
        Message::UserNotLoggedIn();
    } else if (comm._status == "NOK") {
        //If the response is NOK, show a message
        Message::AuctionNotCreated();
    }
}

void CloseCommand::handle(std::vector<std::string> args, Client &receiver) {
    if (args.size() != 1) {
        //If the number of arguments is not 1, throw an exception
        throw CommandArgumentException(_usage);
    }

    std::string AID = args[0];  //Get the AID from the arguments

    if (AID.length() != 3 ||
        !isNumeric(AID)) {  //If the AID is invalid, throw an exception
        throw CommandArgumentException(_usage);
    }

    if (!receiver._user
             .isLoggedIn()) {  //If the user is not logged in, show a message and return
        Message::UserNotLoggedIn();
        return;
    }

    //Create a CloseAuctionCommunication object and set the username, password, and AID
    CloseAuctionCommunication comm;

    comm._uid = receiver._user.getUsername();
    comm._password = receiver._user.getPassword();
    comm._aid = AID;

    receiver.processRequest(
        comm);  //Send the request to the server, receiving its response

    if (comm._status == "OK") {
        //If the response is OK, show a message
        Message::AuctionClosedSucessfully(AID);
    } else if (comm._status == "NLG") {
        //If the response is NLG, show a message
        Message::UserNotLoggedIn();
    } else if (comm._status == "EAU") {
        //If the response is EAU, show a message
        Message::AuctionNotExists(AID);
    } else if (comm._status == "EOW") {
        //If the response is EOW, show a message
        Message::UserNotOwner();
    } else if (comm._status == "END") {
        //If the response is END, show a message
        Message::AuctionAlreadyEnded(AID);
    } else if (comm._status == "NOK") {
        //If the response is NOK, show a message
        Message::WrongUsernameOrPassword();
    }
}

void ListUserAuctionsCommand::handle(std::vector<std::string> args,
                                     Client &receiver) {
    if (args.size() != 0) {
        //If the number of arguments is not 0, throw an exception
        throw CommandArgumentException(_usage);
    }

    if (!receiver._user.isLoggedIn()) {
        //If the user is not logged in, show a message and return
        Message::UserNotLoggedIn();
        return;
    }

    //Create a ListUserAuctionsCommunication object and set the username
    ListUserAuctionsCommunication listUserAuctionsCommunication;
    listUserAuctionsCommunication._uid = receiver._user.getUsername();

    receiver.processRequest(
        listUserAuctionsCommunication);  //Send the request to the server, receiving its response

    if (listUserAuctionsCommunication._status == "NOK") {
        //If the response is NOK, show a message
        Message::UserNotMadeAuction();
    } else if (listUserAuctionsCommunication._status == "NLG") {
        //If the response is NLG, show a message
        Message::UserNotLoggedIn();
    } else if (listUserAuctionsCommunication._status == "OK") {
        //If the response is OK, show the user and the list of auctions
        Message::ShowUser(listUserAuctionsCommunication._uid);
        Message::ListAuctionsAndState(listUserAuctionsCommunication._auctions);
    }
}

void ListUserBidsCommand::handle(std::vector<std::string> args,
                                 Client &receiver) {
    if (args.size() != 0) {
        //If the number of arguments is not 0, throw an exception
        throw CommandArgumentException(_usage);
    }

    if (!receiver._user.isLoggedIn()) {
        //If the user is not logged in, show a message and return
        Message::UserNotLoggedIn();
        return;
    }

    //Create a ListUserBidsCommunication object and set the username
    ListUserBidsCommunication listUserBidsCommunication;
    listUserBidsCommunication._uid = receiver._user.getUsername();

    receiver.processRequest(
        listUserBidsCommunication);  //Send the request to the server, receiving its response

    if (listUserBidsCommunication._status == "NOK") {
        //If the response is NOK, show a message
        Message::UserHasNoBids();
    } else if (listUserBidsCommunication._status == "NLG") {
        //If the response is NLG, show a message
        Message::UserNotLoggedIn();
    } else if (listUserBidsCommunication._status == "OK") {
        //If the response is OK, show the user and the list of bids
        Message::ShowUser(listUserBidsCommunication._uid);
        Message::ListAuctionsAndState(listUserBidsCommunication._bids);
    }
}

void ListAllAuctionsCommand::handle(std::vector<std::string> args,
                                    Client &receiver) {
    if (args.size() != 0) {
        //If the number of arguments is not 0, throw an exception
        throw CommandArgumentException(_usage);
    }

    //Create a ListAllAuctionsCommunication object
    ListAllAuctionsCommunication listAllAuctionsCommunication;

    //Send the request to the server, receiving its response
    receiver.processRequest(listAllAuctionsCommunication);

    if (listAllAuctionsCommunication._status == "NOK") {
        //If the response is NOK, show a message
        Message::NoAuctionStarted();
    } else if (listAllAuctionsCommunication._status == "OK") {
        //If the response is OK, show the list of auctions
        Message::ListAuctionsAndState(listAllAuctionsCommunication._auctions);
    }
}

void ShowAssetCommand::handle(std::vector<std::string> args, Client &receiver) {
    if (args.size() != 1) {
        //If the number of arguments is not 1, throw an exception
        throw CommandArgumentException(_usage);
    }

    std::string AID = args[0];  //Get the AID from the arguments

    if (AID.length() != 3 || !isNumeric(AID)) {
        //If the AID is invalid, throw an exception
        throw CommandArgumentException(_usage);
    }

    //Create a ShowAssetCommunication object and set the AID
    ShowAssetCommunication comm;
    comm._aid = AID;

    receiver.processRequest(
        comm);  //Send the request to the server, receiving its response

    if (comm._status == "OK") {
        //If the response is OK, show a message and download the asset
        Message::DownloadAsset(receiver.getDownloadPath() + comm._fileName,
                               comm._fileSize);
        receiver.writeFile(comm._fileName, comm._fileData);
    } else if (comm._status == "NOK") {
        //If the response is NOK, show a message
        Message::ErrorDownloadAsset();
    }
}

void BidCommand::handle(std::vector<std::string> args, Client &receiver) {
    if (args.size() != 2) {
        //If the number of arguments is not 2, throw an exception
        throw CommandArgumentException(_usage);
    }

    //Get the AID and value from the arguments
    std::string AID = args[0];
    std::string value = args[1];

    if (AID.length() != 3 || value.length() > 6) {
        //If the AID is invalid or the value is too big, throw an exception
        throw CommandArgumentException(_usage);
    }

    if (!isNumeric(AID) || !isNumeric(value)) {
        //If the AID or value is not numeric, throw an exception
        throw CommandArgumentException(_usage);
    }

    if (!receiver._user.isLoggedIn()) {
        //If the user is not logged in, show a message and return
        Message::UserNotLoggedIn();
        return;
    }

    //Create a BidCommunication object and set the username, password, AID, and value
    BidCommunication comm;

    comm._uid = receiver._user.getUsername();
    comm._password = receiver._user.getPassword();
    comm._aid = AID;
    comm._value = atoi(value.c_str());

    receiver.processRequest(
        comm);  //Send the request to the server, receiving its response

    if (comm._status == "NLG") {
        //If the response is NLG, show a message
        Message::UserNotLoggedIn();
    } else if (comm._status == "NOK") {
        //If the response is NOK, show a message
        Message::AuctionAlreadyEnded(AID);
    } else if (comm._status == "ACC") {
        //If the response is ACC, show a message
        Message::UserSucessfullyBid(value, AID);
    } else if (comm._status == "REF") {
        //If the response is REF, show a message
        Message::HigherValueBid();
    } else if (comm._status == "ILG") {
        //If the response is ILG, show a message
        Message::BidOwnAuctions();
    }
}

void ShowRecordCommand::handle(std::vector<std::string> args,
                               Client &receiver) {
    if (args.size() != 1) {
        //If the number of arguments is not 1, throw an exception
        throw CommandArgumentException(_usage);
    }

    //Get the AID from the arguments
    std::string AID = args[0];

    if (AID.length() != 3 && !isNumeric(AID)) {
        //If the AID is invalid, throw an exception
        throw CommandArgumentException(_usage);
    }

    //Create a ShowRecordCommunication object and set the AID
    ShowRecordCommunication showRecordCommunication;

    showRecordCommunication._aid = AID;

    receiver.processRequest(
        showRecordCommunication);  //Send the request to the server, receiving its response

    if (showRecordCommunication._status == "NOK") {
        //If the response is NOK, show a message
        Message::AuctionNotExists(AID);
    } else if (showRecordCommunication._status == "OK") {
        //If the response is OK, show the record
        Message::ShowRecordHeader(showRecordCommunication._aid,
                                  showRecordCommunication._hostUid,
                                  showRecordCommunication._auctionName,
                                  showRecordCommunication._assetFname,
                                  showRecordCommunication._startValue,
                                  showRecordCommunication._startDateTime,
                                  showRecordCommunication._timeActive);
        Message::ShowRecordBids(showRecordCommunication._bidderUids,
                                showRecordCommunication._bidValues,
                                showRecordCommunication._bidDateTime,
                                showRecordCommunication._bidSecTimes);
        if (showRecordCommunication._hasEnded) {
            //If the auction has ended, show a message
            Message::AuctionRecordEnded(showRecordCommunication._endDateTime,
                                        showRecordCommunication._endSecTime);
        }
    }
}
