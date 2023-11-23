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
        std::cout << "User already logged in" << std::endl;
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
        std::cout << "User sucessfully logged in" << std::endl;
        reciever._user.logIn(UID, password);
    } else if (loginCommunication._status == "NOK") {
        std::cout << "Wrong password" << std::endl;
    } else if (loginCommunication._status == "REG") {
        std::cout << "User successfully registered" << std::endl;
        reciever._user.logIn(UID, password);
    }
}

void LogoutCommand::handle(std::vector<std::string> args, Client &reciever) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    if (!reciever._user.isLoggedIn()) {
        std::cout << "User is not logged in" << std::endl;
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
        std::cout << "User sucessfully logged out" << std::endl;
        reciever._user.logOut();
    } else if (logoutCommunication._status == "NOK") {
        std::cout << "User is not logged in" << std::endl;
    } else if (logoutCommunication._status == "UNR") {
        std::cout << "User is not registered" << std::endl;
    }
}

void UnregisterCommand::handle(std::vector<std::string> args,
                               Client &reciever) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    if (!reciever._user.isLoggedIn()) {
        std::cout << "User is not logged in" << std::endl;
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
        std::cout << "User sucessfully unregistered" << std::endl;
        reciever._user.logOut();
    } else if (unregisterCommunication._status == "NOK") {
        std::cout << "User is not logged in" << std::endl;
    } else if (unregisterCommunication._status == "UNR") {
        std::cout << "User is not registered" << std::endl;
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
        std::cout << "You need to be logged in" << std::endl;
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

    try {
        reciever.processRequest(comm);
    } catch (...) {
        // Handling
    }

    if (comm._status == "OK") {
        std::cout << "Auction successfully created. AID: " << comm._aid
                  << std::endl;
    } else if (comm._status == "NLG") {
        std::cout << "You need to be logged in" << std::endl;
    } else if (comm._status == "NOK") {
        std::cout << "Auction was not able to be created" << std::endl;
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
        std::cout << "You need to be logged out to close an auction"
                  << std::endl;
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
        std::cout << "Auction " << AID << " closed successfully" << std::endl;
    } else if (comm._status == "NLG") {
        std::cout << "You need to be logged out to close an auction"
                  << std::endl;
    } else if (comm._status == "EAU") {
        std::cout << "Auction " << AID << " does not exist" << std::endl;
    } else if (comm._status == "EOW") {
        std::cout << "You need to be the owner of the auction to delete it"
                  << std::endl;
    } else if (comm._status == "END") {
        std::cout << "Auction " << AID << " has already ended" << std::endl;
    }
}

void ListUserAuctionsCommand::handle(std::vector<std::string> args,
                                     Client &reciever) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    if (!reciever._user.isLoggedIn()) {
        std::cout << "User is not logged in" << std::endl;
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
        std::cout << "User has not made an auction" << std::endl;
    } else if (listUserAuctionsCommunication._status == "NLG") {
        std::cout << "User is not logged in" << std::endl;
    } else if (listUserAuctionsCommunication._status == "OK") {
        for (auto auction : listUserAuctionsCommunication._auctions) {
            std::string state;
            if (auction.second == "0") {
                state = " Status: Not active";
            } else {
                state = " Status: Active";
            }
            std::cout << "Auction ID: " << auction.first << state << std::endl;
        }
    }
}

void ListUserBidsCommand::handle(std::vector<std::string> args,
                                 Client &reciever) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    if (!reciever._user.isLoggedIn()) {
        std::cout << "User is not logged in" << std::endl;
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
        std::cout << "User has bid in any auction" << std::endl;
    } else if (listUserBidsCommunication._status == "NLG") {
        std::cout << "User is not logged in" << std::endl;
    } else if (listUserBidsCommunication._status == "OK") {
        for (auto bids : listUserBidsCommunication._bids) {
            std::string state;
            if (bids.second == "0") {
                state = " Status: Not active";
            } else {
                state = " Status: Active";
            }
            std::cout << "Auction ID: " << bids.first << state << std::endl;
        }
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
        std::cout << "No auctions has been started" << std::endl;
    } else if (listAllAuctionsCommunication._status == "OK") {
        for (auto auction : listAllAuctionsCommunication._auctions) {
            std::string state;
            if (auction.second == "0") {
                state = " Status: Not active";
            } else {
                state = " Status: Active";
            }
            std::cout << "Auction ID: " << auction.first << state << std::endl;
        }
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
        std::cout << "Downloaded file " << comm._fileName << " ("
                  << comm._fileSize << " bytes)" << std::endl;
        reciever.writeFile(comm._fileName, comm._fileData);
    } else if (comm._status == "NOK") {
        std::cout << "There was an error downloading the requested asset"
                  << std::endl;
    }
}

void BidCommand::handle(std::vector<std::string> args, Client &reciever) {
    if (args.size() != 2) {
        throw CommandArgumentException(_usage);
    }

    (void)reciever;

    std::string AID = args[0];
    std::string value = args[0];

    if (AID.length() != 3 || value.length() > 6) {
        throw CommandArgumentException(_usage);
    }

    if (!isNumeric(AID) || !isNumeric(value)) {
        throw CommandArgumentException(_usage);
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
        std::cout << "Auction ID: " << showRecordCommunication._aid
                  << std::endl;
        std::cout << "Host ID: " << showRecordCommunication._hostUid
                  << std::endl;
        std::cout << "Auction Name: " << showRecordCommunication._auctionName
                  << std::endl;
        std::cout << "Asset File Name: " << showRecordCommunication._assetFname
                  << std::endl;
        std::cout << "Start Value: " << showRecordCommunication._startValue
                  << std::endl;
        std::cout << "Start Date and Time: "
                  << showRecordCommunication._startDateTime << std::endl;
        std::cout << "Time of activity: " << showRecordCommunication._timeActive
                  << std::endl;
        std::cout << "---------------------" << std::endl;
        std::cout << "Bids: " << std::endl;
        std::cout << "---------------------" << std::endl;
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
            std::cout << "This auction has ended" << std::endl;
            std::cout << "End Date and Time: "
                      << showRecordCommunication._endDateTime;
            std::cout << "Number of seconds the auction was opened: "
                      << showRecordCommunication._endSecTime << std::endl;
        }
    }
}
