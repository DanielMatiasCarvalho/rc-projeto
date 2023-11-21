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

    (void)reciever;

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
}

void CloseCommand::handle(std::vector<std::string> args, Client &reciever) {
    if (args.size() != 1) {
        throw CommandArgumentException(_usage);
    }

    (void)reciever;

    std::string AID = args[0];

    if (AID.length() != 3) {
        throw CommandArgumentException(_usage);
    }

    if (!isNumeric(AID)) {
        throw CommandArgumentException(_usage);
    }
}

void ListUserAuctionsCommand::handle(std::vector<std::string> args,
                                     Client &reciever) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    (void)reciever;
}

void ListUserBidsCommand::handle(std::vector<std::string> args,
                                 Client &reciever) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    (void)reciever;
}

void ListAllAuctionsCommand::handle(std::vector<std::string> args,
                                    Client &reciever) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    (void)reciever;
}

void ShowAssetCommand::handle(std::vector<std::string> args, Client &reciever) {
    if (args.size() != 1) {
        throw CommandArgumentException(_usage);
    }

    (void)reciever;

    std::string AID = args[0];

    if (AID.length() != 3) {
        throw CommandArgumentException(_usage);
    }

    if (!isNumeric(AID)) {
        throw CommandArgumentException(_usage);
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

    (void)reciever;

    std::string AID = args[0];

    if (AID.length() != 3) {
        throw CommandArgumentException(_usage);
    }

    if (!isNumeric(AID)) {
        throw CommandArgumentException(_usage);
    }
}
