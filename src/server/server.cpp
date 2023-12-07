#include "server.hpp"
#include "command.hpp"

/**
 * @brief Starts and runs the UDP server.
 * 
 * This function initializes and runs the UDP server.
 */
void UDPServer(CommandManager &manager, Server &server);

/**
 * @brief Starts and runs the TCP server.
 * 
 * This function initializes and runs the TCP server.
 */
void TCPServer(CommandManager &manager, Server &server);

int main(int argc, char **argv) {
    Server server(argc, argv);
    CommandManager manager;
    struct sigaction act;
    pid_t pid;

    act.sa_handler = SIG_IGN;
    if (sigaction(SIGCHLD, &act, NULL) == -1) {
        exit(1);
    }

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

    if ((pid = fork()) == -1) {
        exit(1);
    } else if (pid == 0) {
        std::cout << "UDP server started" << std::endl;
        UDPServer(manager, server);
    } else {
        std::cout << "TCP server started" << std::endl;
        TCPServer(manager, server);
    }

    return 0;
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

void UDPServer(CommandManager &manager, Server &server) {
    UdpServer UDPserver(server.getPort());
    while (1) {
        std::stringstream message = UDPserver.receive();
        std::cout << "UDP server received message" << std::endl;
        std::stringstream response;
        manager.readCommand(message, response, server);
        UDPserver.send(response);
    }
}

void TCPServer(CommandManager &manager, Server &server) {
    TcpServer TCPserver(server.getPort());
    while (1) {
        TcpSession session(TCPserver.acceptConnection());
        std::cout << "TCP server accepted connection" << std::endl;
        pid_t pid;
        if ((pid = fork()) == -1) {
            exit(1);
        } else if (pid == 0) {
            std::stringstream message = session.receive();
            std::stringstream response;
            std::cout << "Received" << std::endl;
            manager.readCommand(message, response, server);
            std::cout << "Processed" << std::endl;
            session.send(response);
            std::cout << "Sent" << std::endl;
            exit(0);
        }
    }
}
