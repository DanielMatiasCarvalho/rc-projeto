#include "command.hpp"

void CommandManager::registerCommand(std::shared_ptr<CommandHandler> handler) {
    this->handlers.insert({handler->_name, handler});

    for (auto alias : handler->_alias) {
        this->handlers.insert({alias, handler});
    }
}

void CommandManager::readCommand(ClientState state) {
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

    handler->second->handle(args, state);
}

void LoginCommand::handle(std::vector<std::string> args, ClientState state) {
    if (args.size() != 2) {
        throw CommandArgumentException(_usage);
    }

    (void)state;

    std::string UID = args[0];
    std::string password = args[1];

    if (UID.length() != 6 || password.length() != 8) {
        throw CommandArgumentException(_usage);
    }
}

void LogoutCommand::handle(std::vector<std::string> args, ClientState state) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    (void)state;

}

void UnregisterCommand::handle(std::vector<std::string> args, ClientState state) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    (void)state;

}

void ExitCommand::handle(std::vector<std::string> args, ClientState state) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    (void)state;

}

void OpenCommand::handle(std::vector<std::string> args, ClientState state) {
    if (args.size() != 4) {
        throw CommandArgumentException(_usage);
    }

    (void)state;

    std::string name = args[0];
    std::string asset_fname = args[1];
    std::string start_value = args[2];
    std::string timeactive = args[3];

    if (name.length() > 10 || asset_fname.length() > 24 || start_value.length() > 6 || timeactive.length() > 5) {
        throw CommandArgumentException(_usage);
    }
}

void CloseCommand::handle(std::vector<std::string> args, ClientState state) {
    if (args.size() != 1) {
        throw CommandArgumentException(_usage);
    }

    (void)state;

    std::string AID = args[0];

    if (AID.length() != 3) {
        throw CommandArgumentException(_usage);
    }
}

void ListUserAuctionsCommand::handle(std::vector<std::string> args, ClientState state) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    (void)state;

}

void ListUserBidsCommand::handle(std::vector<std::string> args, ClientState state) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    (void)state;

}

void ListAllAuctionsCommand::handle(std::vector<std::string> args, ClientState state) {
    if (args.size() != 0) {
        throw CommandArgumentException(_usage);
    }

    (void)state;

}

void ShowAssetCommand::handle(std::vector<std::string> args, ClientState state) {
    if (args.size() != 1) {
        throw CommandArgumentException(_usage);
    }

    (void)state;

    std::string AID = args[0];

    if (AID.length() != 3) {
        throw CommandArgumentException(_usage);
    }
}

void BidCommand::handle(std::vector<std::string> args, ClientState state) {
    if (args.size() != 2) {
        throw CommandArgumentException(_usage);
    }

    (void)state;

    std::string AID = args[0];
    std::string value = args[0];

    if (AID.length() != 3 || value.length() > 6) {
        throw CommandArgumentException(_usage);
    }
}

void ShowRecordCommand::handle(std::vector<std::string> args, ClientState state) {
    if (args.size() != 1) {
        throw CommandArgumentException(_usage);
    }

    (void)state;

    std::string AID = args[0];

    if (AID.length() != 3) {
        throw CommandArgumentException(_usage);
    }
}