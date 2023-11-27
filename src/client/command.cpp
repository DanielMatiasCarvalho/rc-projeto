#include "command.hpp"

void CommandManager::registerCommand(std::shared_ptr<CommandHandler> handler) {
    this->handlers.insert({handler->_name, handler});

    for (auto alias : handler->_alias) {
        this->handlers.insert({alias, handler});
    }
}

void CommandManager::readCommand(Client &reciever) {
    std::cout << "> ";

    std::string line;
    std::getline(std::cin, line);

    if (std::cin.eof()) {
        return;
    }

    if (line.length() == 0) {
        return;
    }

    std::string name;
    std::vector<std::string> args;

    auto position = line.find(" ");

    if (position == std::string::npos) {
        name = line;
        line = "";
    } else {
        name = line.substr(0, position);
        line.erase(0, position + 1);
    }

    position = line.find(" ");
    while (position != std::string::npos) {
        args.push_back(line.substr(0, position));
        line.erase(0, position + 1);
        position = line.find(" ");
    }

    if (line.length() != 0) {
        args.push_back(line);
    }

    auto handler = this->handlers.find(name);
    if (handler == this->handlers.end()) {
        throw UnknownCommandException();
    }

    handler->second->handle(args, reciever);
}

void LoginCommand::handle(std::vector<std::string> args, Client &reciever) {
    if (args.size() != 2) {
        throw CommandArgumentException(_usage);
    }

    std::string UID = args[0];
    std::string password = args[1];

    if (UID.length() != 6 || password.length() != 8 || !isNumeric(UID)) {
        throw CommandArgumentException(_usage);
    }

    if (reciever._user.isLoggedIn()) {
        Message::UserAlreadyLoggedIn();
        return;
    }

    LoginCommunication loginCommunication;
    loginCommunication._uid = UID;
    loginCommunication._password = password;

    try {
        reciever.processRequest(loginCommunication);
    } catch (...) {
        // Something
    }

    if (loginCommunication._status == "OK") {
        Message::UserLoginSuccess();
        reciever._user.logIn(UID, password);
    } else if (loginCommunication._status == "NOK") {
        Message::WrongPassword();
    } else if (loginCommunication._status == "REG") {
        Message::UserRegisterSuccess();
        reciever._user.logIn(UID, password);
    }
}

void LogoutCommand::handle(std::vector<std::string> args, Client &reciever) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    if (!reciever._user.isLoggedIn()) {
        Message::UserNotLoggedIn();
        return;
    }

    LogoutCommunication logoutCommunication;
    logoutCommunication._uid = reciever._user.getUsername();
    logoutCommunication._password = reciever._user.getPassword();

    try {
        reciever.processRequest(logoutCommunication);
    } catch (...) {
        // Something
    }

    if (logoutCommunication._status == "OK") {
        Message::UserLogoutSucess();
        reciever._user.logOut();
    } else if (logoutCommunication._status == "NOK") {
        Message::UserNotLoggedIn();
    } else if (logoutCommunication._status == "UNR") {
        Message::UserNotRegistered();
    }
}

void UnregisterCommand::handle(std::vector<std::string> args,
                               Client &reciever) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    if (!reciever._user.isLoggedIn()) {
        Message::UserNotLoggedIn();
        return;
    }

    UnregisterCommunication unregisterCommunication;
    unregisterCommunication._uid = reciever._user.getUsername();
    unregisterCommunication._password = reciever._user.getPassword();

    try {
        reciever.processRequest(unregisterCommunication);
    } catch (...) {
        // Something
    }

    if (unregisterCommunication._status == "OK") {
        Message::UserUnregisterSucess();
        reciever._user.logOut();
    } else if (unregisterCommunication._status == "NOK") {
        Message::UserNotLoggedIn();
    } else if (unregisterCommunication._status == "UNR") {
        Message::UserNotRegistered();
    }
}

void ExitCommand::handle(std::vector<std::string> args, Client &reciever) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    if (reciever._user.isLoggedIn()) {
        std::cout << "User is logged in" << std::endl;
        return;
    }

    reciever._toExit = true;
}

void OpenCommand::handle(std::vector<std::string> args, Client &reciever) {
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

    if (!reciever._user.isLoggedIn()) {
        Message::UserNotLoggedIn();
        return;
    }

    OpenAuctionCommunication comm;

    comm._uid = reciever._user.getUsername();
    comm._password = reciever._user.getPassword();
    comm._name = name;
    comm._startValue = atoi(start_value.c_str());
    comm._timeActive = atoi(timeactive.c_str());
    comm._fileName = asset_fname;
    comm._fileSize = reciever.getFileSize(asset_fname);
    comm._fileData = reciever.readFile(asset_fname);

    if (comm._fileSize > PROTOCOL_MAX_FILE_SIZE) {
        std::cout << "File too big" << std::endl;
        return;
    }

    try {
        reciever.processRequest(comm);
    } catch (...) {
        // Handling
    }

    if (comm._status == "OK") {
        Message::AuctionCreated(comm._aid);
    } else if (comm._status == "NLG") {
        Message::UserNotLoggedIn();
    } else if (comm._status == "NOK") {
        Message::AuctionNotCreated();
    }
}

void CloseCommand::handle(std::vector<std::string> args, Client &reciever) {
    if (args.size() != 1) {
        throw CommandArgumentException(_usage);
    }

    std::string AID = args[0];

    if (AID.length() != 3 || !isNumeric(AID)) {
        throw CommandArgumentException(_usage);
    }

    if (!reciever._user.isLoggedIn()) {
        Message::UserNotLoggedIn();
        return;
    }

    CloseAuctionCommunication comm;

    comm._uid = reciever._user.getUsername();
    comm._password = reciever._user.getPassword();
    comm._aid = AID;

    try {
        reciever.processRequest(comm);
    } catch (...) {
        // exception handling
    }

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
                                     Client &reciever) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    if (!reciever._user.isLoggedIn()) {
        Message::UserNotLoggedIn();
        return;
    }

    ListUserAuctionsCommunication listUserAuctionsCommunication;
    listUserAuctionsCommunication._uid = reciever._user.getUsername();

    try {
        reciever.processRequest(listUserAuctionsCommunication);
    } catch (...) {
        // Something
    }

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
                                 Client &reciever) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    if (!reciever._user.isLoggedIn()) {
        Message::UserNotLoggedIn();
        return;
    }

    ListUserBidsCommunication listUserBidsCommunication;
    listUserBidsCommunication._uid = reciever._user.getUsername();

    try {
        reciever.processRequest(listUserBidsCommunication);
    } catch (...) {
        // Something
    }

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
                                    Client &reciever) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    ListAllAuctionsCommunication listAllAuctionsCommunication;

    try {
        reciever.processRequest(listAllAuctionsCommunication);
    } catch (...) {
        // Something
    }

    if (listAllAuctionsCommunication._status == "NOK") {
        Message::NoAuctionStarted();
    } else if (listAllAuctionsCommunication._status == "OK") {
        Message::ListAuctionsAndState(listAllAuctionsCommunication._auctions);
    }
}

void ShowAssetCommand::handle(std::vector<std::string> args, Client &reciever) {
    if (args.size() != 1) {
        throw CommandArgumentException(_usage);
    }

    std::string AID = args[0];

    if (AID.length() != 3 || !isNumeric(AID)) {
        throw CommandArgumentException(_usage);
    }

    ShowAssetCommunication comm;
    comm._aid = AID;

    try {
        reciever.processRequest(comm);
    } catch (...) {
        // Exception handling
    }

    if (comm._status == "OK") {
        Message::DownloadAsset(comm._fileName, comm._fileSize);
        reciever.writeFile(comm._fileName, comm._fileData);
    } else if (comm._status == "NOK") {
        Message::ErrorDownloadAsset();
    }
}

void BidCommand::handle(std::vector<std::string> args, Client &reciever) {
    if (args.size() != 2) {
        throw CommandArgumentException(_usage);
    }

    std::string AID = args[0];
    std::string value = args[0];

    if (AID.length() != 3 || value.length() > 6) {
        throw CommandArgumentException(_usage);
    }

    if (!isNumeric(AID) || !isNumeric(value)) {
        throw CommandArgumentException(_usage);
    }

    if (!reciever._user.isLoggedIn()) {
        Message::UserNotLoggedIn();
        return;
    }

    BidCommunication comm;

    comm._uid = reciever._user.getUsername();
    comm._password = reciever._user.getPassword();
    comm._aid = AID;
    comm._value = atoi(value.c_str());

    try {
        reciever.processRequest(comm);
    } catch (...) {
        // handling
    }

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
                               Client &reciever) {
    if (args.size() != 1) {
        throw CommandArgumentException(_usage);
    }

    std::string AID = args[0];

    if (AID.length() != 3 && !isNumeric(AID)) {
        throw CommandArgumentException(_usage);
    }

    ShowRecordCommunication showRecordCommunication;

    showRecordCommunication._aid = AID;

    try {
        reciever.processRequest(showRecordCommunication);
    } catch (...) {
        // Something
    }

    if (showRecordCommunication._status == "NOK") {
        std::cout << "The auction you requested does not exist" << std::endl;
    } else if (showRecordCommunication._status == "OK") {
        /*TODO: CHECK DATE AND TIME PRINTS*/
        Message::ShowRecordHeader(showRecordCommunication._aid, showRecordCommunication._hostUid,
                                   showRecordCommunication._auctionName,  showRecordCommunication._assetFname,
                                   showRecordCommunication._startValue, showRecordCommunication._startDateTime,
                                   showRecordCommunication._timeActive);
        long unsigned int size = showRecordCommunication._bidderUids.size();
        for (long unsigned int i = 0; i < size; i++) {
            std::cout << "Bidder ID: " << showRecordCommunication._bidderUids[i]
                      << std::endl;
            std::cout << "\tBid Value: "
                      << showRecordCommunication._bidValues[i] << std::endl;
            std::cout << "\tBid Date and Time: "
                      << showRecordCommunication._bidDateTime[i] << std::endl;
            std::cout << "\tBid Time after the opening of the auction: "
                      << showRecordCommunication._bidSecTimes[i] << std::endl;
        }
        std::cout << "---------------------" << std::endl;
        if (showRecordCommunication._hasEnded) {
            Message::AuctionRecordEnded(showRecordCommunication._endDateTime, 
                                        showRecordCommunication._endSecTime);
        }
    }
}
