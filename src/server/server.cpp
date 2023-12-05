#include "server.hpp"

int main(int argc, char **argv) {
    Server server(argc, argv);
    CommandManager manager;

    manager.registerCommand(std::make_shared<LoginCommand>());
    manager.registerCommand(std::make_shared<LogoutCommand>());
    manager.registerCommand(std::make_shared<UnregisterCommand>());
    manager.registerCommand(std::make_shared<OpenCommand>());
    manager.registerCommand(std::make_shared<CloseCommand>());
    manager.registerCommand(std::make_shared<ListUserAuctionsCommand>());
    manager.registerCommand(std::make_shared<ListUserBidsCommand>());
    manager.registerCommand(std::make_shared<ListAllAuctionsCommand>());
    manager.registerCommand(std::make_shared<ShowAssetCommand>());
    manager.registerCommand(std::make_shared<BidCommand>());
    manager.registerCommand(std::make_shared<ShowRecordCommand>());

    /*
    TcpServer tcpServer(server.getPort());
    while (1) {
        TcpSession session(tcpServer.acceptConnection());
        if (fork() == 0) {
            std::stringstream message = session.receive();
            session.send(message);
            break;
        }
    }
    */

    return 1;
}

/**
 * @brief Constructs a Server object.
 * 
 * This constructor initializes the Server object with the provided command line arguments.
 * It parses the command line arguments and sets the port and verbosity options accordingly.
 * 
 * @param argc The number of command line arguments.
 * @param argv An array of command line arguments.
 */
Server::Server(int argc, char **argv) {
    char c;
    //Parse command line arguments
    while ((c = (char)getopt(argc, argv, "p:v")) != -1) {
        switch (c) {
            case 'p':  //Sets the port
                _port = optarg;
                break;
            case 'v':  //Sets the verbosity
                _verbose = true;
                break;
            default:
                break;
        }
    }

    _database = std::make_unique<Database>("database");
}

/**
 * @brief Displays the information about the server.
 * 
 * This function prints the port number and verbosity level of the server.
 */
void Server::ShowInfo() {
    std::cout << "Port: " << _port << std::endl
              << "Verbose: " << _verbose << std::endl;
}

/**
 * Displays a message if the server is in verbose mode.
 *
 * @param message The message to be displayed.
 */
void Server::showMessage(std::string message) {
    if (_verbose)
        std::cout << message << std::endl;
}
