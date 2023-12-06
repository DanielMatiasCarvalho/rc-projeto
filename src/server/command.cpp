#include "command.hpp"

/**
 * Registers a command handler.
 *
 * @param handler The command handler to register.
 */
void CommandManager::registerCommand(std::shared_ptr<CommandHandler> handler) {
    this->_handlers.insert({handler->_code, handler});
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
void CommandManager::readCommand(std::stringstream &message,
                                 std::stringstream &response,
                                 Server &receiver) {
    std::string code;
    for (size_t i = 0; i < 3; i++) {  //Reads the 3 digit code from the message
        char c = (char)message.get();
        code.push_back(c);
    }

    auto handler =
        this->_handlers.find(code);  //Finds the correct handler for the command
    if (handler ==
        this->_handlers
            .end()) {  //If the handler is not found, the command is not valid
        std::cout << "Command not found" << std::endl;
        return;
    }

    try {
        handler->second->handle(
            message, response,
            receiver);  //Executes the command on the correct handler
    } catch (ProtocolException const &e) {
        std::string str = PROTOCOL_ERROR_IDENTIFIER;
        for (auto c : str) {
            response.put(c);
        }
        response.put('\n');
    }
}

void LoginCommand::handle(std::stringstream &message,
                          std::stringstream &response, Server &receiver) {

    LoginCommunication loginCommunication;
    try {
        loginCommunication.decodeRequest(message);
        if (receiver._database->checkLoggedIn(loginCommunication._uid,
                                              loginCommunication._password)) {
            //O pdf não menciona, mas meti que se já tiver logged in, devolve OK
            loginCommunication._status = "OK";
        } else {
            if (!receiver._database->loginUser(loginCommunication._uid,
                                               loginCommunication._password)) {
                loginCommunication._status = "OK";
            } else {
                loginCommunication._status = "REG";
            }
        }
    } catch (LoginException const &e) {
        loginCommunication._status = "NOK";
    }
    response = loginCommunication.encodeResponse();
}

void LogoutCommand::handle(std::stringstream &message,
                           std::stringstream &response, Server &receiver) {
    (void)receiver;
    (void)message;
    (void)response;
}

void UnregisterCommand::handle(std::stringstream &message,
                               std::stringstream &response, Server &receiver) {
    (void)receiver;
    (void)message;
    (void)response;
}

void ListUserAuctionsCommand::handle(std::stringstream &message,
                                     std::stringstream &response,
                                     Server &receiver) {
    (void)receiver;
    (void)message;
    (void)response;
}

void ListUserBidsCommand::handle(std::stringstream &message,
                                 std::stringstream &response,
                                 Server &receiver) {
    (void)receiver;
    (void)message;
    (void)response;
}

void ListAllAuctionsCommand::handle(std::stringstream &message,
                                    std::stringstream &response,
                                    Server &receiver) {
    ListAllAuctionsCommunication listAllAuctionsCommunication;
    listAllAuctionsCommunication.decodeRequest(message);

    (void)receiver;
    (void)message;
    (void)response;
}

void ShowRecordCommand::handle(std::stringstream &message,
                               std::stringstream &response, Server &receiver) {
    (void)receiver;
    (void)message;
    (void)response;
}

void OpenCommand::handle(std::stringstream &message,
                         std::stringstream &response, Server &receiver) {
    (void)receiver;
    (void)message;
    (void)response;
}

void CloseCommand::handle(std::stringstream &message,
                          std::stringstream &response, Server &receiver) {
    (void)receiver;
    (void)message;
    (void)response;
}

void ShowAssetCommand::handle(std::stringstream &message,
                              std::stringstream &response, Server &receiver) {
    (void)receiver;
    (void)message;
    (void)response;
}

void BidCommand::handle(std::stringstream &message, std::stringstream &response,
                        Server &receiver) {
    (void)receiver;
    (void)message;
    (void)response;
}