#include "command.hpp"

void CommandManager::registerHandler(std::shared_ptr<CommandHandler> handler) {
    this->_handlers.insert({handler->_code, handler});
}

void CommandManager::readCommand(std::stringstream message, Server &receiver) {
    std::string code;
    for (size_t i = 0; i < 3; i++) {
        char c = (char)message.get();
        code.push_back(c);
    }

    auto handler = this->_handlers.find(code);
    if (handler == this->_handlers.end()) {
        std::cout << "Command not found" << std::endl;
        return;
    }

    handler->second->handle(message, receiver);
}