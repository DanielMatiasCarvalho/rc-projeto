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