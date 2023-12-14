#include "command.hpp"

void CommandManager::registerCommand(std::shared_ptr<CommandHandler> handler) {
    this->handlers.insert({handler->_name, handler});

    for (auto alias : handler->_alias) {
        this->handlers.insert({alias, handler});
    }
}

void CommandManager::readCommand(std::string command, Client &receiver) {
    if (command.length() == 0) {
        return;
    }

    std::string name;
    std::vector<std::string> args;

    auto position = command.find(" ");

    if (position == std::string::npos) {
        name = command;
        command = "";
    } else {
        name = command.substr(0, position);
        command.erase(0, position + 1);
    }

    position = command.find(" ");
    while (position != std::string::npos) {
        args.push_back(command.substr(0, position));
        command.erase(0, position + 1);
        position = command.find(" ");
    }

    if (command.length() != 0) {
        args.push_back(command);
    }

    auto handler = this->handlers.find(name);
    if (handler == this->handlers.end()) {
        throw UnknownCommandException();
    }

    handler->second->handle(args, receiver);
}

void LoginCommand::handle(std::vector<std::string> args, Client &receiver) {
    if (args.size() != 2) {
        throw CommandArgumentException(_usage);
    }

    std::string UID = args[0];
    std::string password = args[1];

    if (UID.length() != 6 || password.length() != 8 || !isNumeric(UID)) {
        throw CommandArgumentException(_usage);
    }

    if (receiver._user.isLoggedIn()) {
        Message::UserAlreadyLoggedIn();
        return;
    }

    LoginCommunication loginCommunication;
    loginCommunication._uid = UID;
    loginCommunication._password = password;

    receiver.processRequest(loginCommunication);

    if (loginCommunication._status == "OK") {
        Message::UserLoginSuccess();
        receiver._user.logIn(UID, password);
    } else if (loginCommunication._status == "NOK") {
        Message::WrongPassword();
    } else if (loginCommunication._status == "REG") {
        Message::UserRegisterSuccess();
        receiver._user.logIn(UID, password);
    }
}

void LogoutCommand::handle(std::vector<std::string> args, Client &receiver) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    if (!receiver._user.isLoggedIn()) {
        Message::UserNotLoggedIn();
        return;
    }

    LogoutCommunication logoutCommunication;
    logoutCommunication._uid = receiver._user.getUsername();
    logoutCommunication._password = receiver._user.getPassword();

    receiver.processRequest(logoutCommunication);

    if (logoutCommunication._status == "OK") {
        Message::UserLogoutSucess();
        receiver._user.logOut();
    } else if (logoutCommunication._status == "NOK") {
        Message::UserNotLoggedIn();
    } else if (logoutCommunication._status == "UNR") {
        Message::UserNotRegistered();
    }
}

void UnregisterCommand::handle(std::vector<std::string> args,
                               Client &receiver) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    if (!receiver._user.isLoggedIn()) {
        Message::UserNotLoggedIn();
        return;
    }

    UnregisterCommunication unregisterCommunication;
    unregisterCommunication._uid = receiver._user.getUsername();
    unregisterCommunication._password = receiver._user.getPassword();

    receiver.processRequest(unregisterCommunication);

    if (unregisterCommunication._status == "OK") {
        Message::UserUnregisterSucess();
        receiver._user.logOut();
    } else if (unregisterCommunication._status == "NOK") {
        Message::UserNotLoggedIn();
    } else if (unregisterCommunication._status == "UNR") {
        Message::UserNotRegistered();
    }
}

void ExitCommand::handle(std::vector<std::string> args, Client &receiver) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    if (receiver._user.isLoggedIn()) {
        std::cout << "User is logged in" << std::endl;
        return;
    }

    receiver._toExit = true;
}

void OpenCommand::handle(std::vector<std::string> args, Client &receiver) {
    if (args.size() != 4) {
        throw CommandArgumentException(_usage);
    }

    std::string name = args[0];
    std::string asset_fname = args[1];
    std::string start_value = args[2];
    std::string timeactive = args[3];

    if (name.length() > 10 || asset_fname.length() > 24 ||
        start_value.length() > 6 || timeactive.length() > 5) {
        throw CommandArgumentException(_usage);
    }

    if (!isNumeric(start_value) || !isNumeric(timeactive)) {
        throw CommandArgumentException(_usage);
    }

    if (!receiver._user.isLoggedIn()) {
        Message::UserNotLoggedIn();
        return;
    }

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
        std::cout << "File too big" << std::endl;
        return;
    }

    receiver.processRequest(comm);

    if (comm._status == "OK") {
        Message::AuctionCreated(comm._aid);
    } else if (comm._status == "NLG") {
        Message::UserNotLoggedIn();
    } else if (comm._status == "NOK") {
        Message::AuctionNotCreated();
    }
}

void CloseCommand::handle(std::vector<std::string> args, Client &receiver) {
    if (args.size() != 1) {
        throw CommandArgumentException(_usage);
    }

    std::string AID = args[0];

    if (AID.length() != 3 || !isNumeric(AID)) {
        throw CommandArgumentException(_usage);
    }

    if (!receiver._user.isLoggedIn()) {
        Message::UserNotLoggedIn();
        return;
    }

    CloseAuctionCommunication comm;

    comm._uid = receiver._user.getUsername();
    comm._password = receiver._user.getPassword();
    comm._aid = AID;

    receiver.processRequest(comm);

    if (comm._status == "OK") {
        Message::AuctionClosedSucessfully(AID);
    } else if (comm._status == "NLG") {
        Message::UserNotLoggedIn();
    } else if (comm._status == "EAU") {
        Message::AuctionNotExists(AID);
    } else if (comm._status == "EOW") {
        Message::UserNotOwner();
    } else if (comm._status == "END") {
        Message::AuctionAlreadyEnded(AID);
    }
}

void ListUserAuctionsCommand::handle(std::vector<std::string> args,
                                     Client &receiver) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    if (!receiver._user.isLoggedIn()) {
        Message::UserNotLoggedIn();
        return;
    }

    ListUserAuctionsCommunication listUserAuctionsCommunication;
    listUserAuctionsCommunication._uid = receiver._user.getUsername();

    receiver.processRequest(listUserAuctionsCommunication);

    if (listUserAuctionsCommunication._status == "NOK") {
        Message::UserNotMadeAuction();
    } else if (listUserAuctionsCommunication._status == "NLG") {
        Message::UserNotLoggedIn();
    } else if (listUserAuctionsCommunication._status == "OK") {
        Message::ShowUser(listUserAuctionsCommunication._uid);
        Message::ListAuctionsAndState(listUserAuctionsCommunication._auctions);
    }
}

void ListUserBidsCommand::handle(std::vector<std::string> args,
                                 Client &receiver) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    if (!receiver._user.isLoggedIn()) {
        Message::UserNotLoggedIn();
        return;
    }

    ListUserBidsCommunication listUserBidsCommunication;
    listUserBidsCommunication._uid = receiver._user.getUsername();

    receiver.processRequest(listUserBidsCommunication);

    if (listUserBidsCommunication._status == "NOK") {
        Message::UserHasNoBids();
    } else if (listUserBidsCommunication._status == "NLG") {
        Message::UserNotLoggedIn();
    } else if (listUserBidsCommunication._status == "OK") {
        Message::ShowUser(listUserBidsCommunication._uid);
        Message::ListAuctionsAndState(listUserBidsCommunication._bids);
    }
}

void ListAllAuctionsCommand::handle(std::vector<std::string> args,
                                    Client &receiver) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    ListAllAuctionsCommunication listAllAuctionsCommunication;

    receiver.processRequest(listAllAuctionsCommunication);

    if (listAllAuctionsCommunication._status == "NOK") {
        Message::NoAuctionStarted();
    } else if (listAllAuctionsCommunication._status == "OK") {
        Message::ListAuctionsAndState(listAllAuctionsCommunication._auctions);
    }
}

void ShowAssetCommand::handle(std::vector<std::string> args, Client &receiver) {
    if (args.size() != 1) {
        throw CommandArgumentException(_usage);
    }

    std::string AID = args[0];

    if (AID.length() != 3 || !isNumeric(AID)) {
        throw CommandArgumentException(_usage);
    }

    ShowAssetCommunication comm;
    comm._aid = AID;

    receiver.processRequest(comm);

    if (comm._status == "OK") {
        Message::DownloadAsset(comm._fileName, comm._fileSize);
        receiver.writeFile(comm._fileName, comm._fileData);
    } else if (comm._status == "NOK") {
        Message::ErrorDownloadAsset();
    }
}

void BidCommand::handle(std::vector<std::string> args, Client &receiver) {
    if (args.size() != 2) {
        throw CommandArgumentException(_usage);
    }

    std::string AID = args[0];
    std::string value = args[1];

    if (AID.length() != 3 || value.length() > 6) {
        throw CommandArgumentException(_usage);
    }

    if (!isNumeric(AID) || !isNumeric(value)) {
        throw CommandArgumentException(_usage);
    }

    if (!receiver._user.isLoggedIn()) {
        Message::UserNotLoggedIn();
        return;
    }

    BidCommunication comm;

    comm._uid = receiver._user.getUsername();
    comm._password = receiver._user.getPassword();
    comm._aid = AID;
    comm._value = atoi(value.c_str());

    receiver.processRequest(comm);

    if (comm._status == "NLG") {
        Message::UserNotLoggedIn();
    } else if (comm._status == "NOK") {
        Message::AuctionAlreadyEnded(AID);
    } else if (comm._status == "ACC") {
        Message::UserSucessfullyBid(value, AID);
    } else if (comm._status == "REF") {
        Message::HigherValueBid();
    } else if (comm._status == "ILG") {
        Message::BidOwnAuctions();
    }
}

void ShowRecordCommand::handle(std::vector<std::string> args,
                               Client &receiver) {
    if (args.size() != 1) {
        throw CommandArgumentException(_usage);
    }

    std::string AID = args[0];

    if (AID.length() != 3 && !isNumeric(AID)) {
        throw CommandArgumentException(_usage);
    }

    ShowRecordCommunication showRecordCommunication;

    showRecordCommunication._aid = AID;

    receiver.processRequest(showRecordCommunication);

    if (showRecordCommunication._status == "NOK") {
        std::cout << "The auction you requested does not exist" << std::endl;
    } else if (showRecordCommunication._status == "OK") {
        /*TODO: CHECK DATE AND TIME PRINTS*/
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
            Message::AuctionRecordEnded(showRecordCommunication._endDateTime,
                                        showRecordCommunication._endSecTime);
        }
    }
}
