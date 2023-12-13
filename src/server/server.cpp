#include "server.hpp"
#include "command.hpp"

/**
 * @brief Starts and runs the UDP server.
 * 
 * This function initializes and runs the UDP server.
 */
void UDPServer(UdpServer &udpServer, CommandManager &manager, Server &server);

/**
 * @brief Starts and runs the TCP server.
 * 
 * This function initializes and runs the TCP server.
 */
void TCPServer(TcpServer &tcpServer, CommandManager &manager, Server &server);

int main(int argc, char **argv) {
    Server server(argc, argv);
    CommandManager manager;
    struct sigaction act;
    pid_t pid;

    act.sa_handler = SIG_IGN;
    if (sigaction(SIGCHLD, &act, NULL) == -1) {
        exit(1);
    }
    if (sigaction(SIGPIPE, &act, NULL) == -1) {
        exit(1);
    }

    manager.registerCommand(std::make_shared<LoginCommand>(), false);
    manager.registerCommand(std::make_shared<LogoutCommand>(), false);
    manager.registerCommand(std::make_shared<UnregisterCommand>(), false);
    manager.registerCommand(std::make_shared<OpenCommand>(), true);
    manager.registerCommand(std::make_shared<CloseCommand>(), true);
    manager.registerCommand(std::make_shared<ListUserAuctionsCommand>(), false);
    manager.registerCommand(std::make_shared<ListUserBidsCommand>(), false);
    manager.registerCommand(std::make_shared<ListAllAuctionsCommand>(), false);
    manager.registerCommand(std::make_shared<ShowAssetCommand>(), true);
    manager.registerCommand(std::make_shared<BidCommand>(), true);
    manager.registerCommand(std::make_shared<ShowRecordCommand>(), false);

    try {
        UdpServer udpServer(server.getPort());
        TcpServer tcpServer(server.getPort());

        server.showMessage("Listening on port " + server.getPort());

        if ((pid = fork()) == -1) {
            exit(1);
        } else if (pid == 0) {
            tcpServer.close();
            server.showMessage("UDP server started");
            UDPServer(udpServer, manager, server);
        } else {
            udpServer.close();
            server.showMessage("TCP server started");
            TCPServer(tcpServer, manager, server);
        }
    } catch (SocketException const &e) {
        std::cout << "Server could not connect to the sockets. Ensure that the "
                     "port is not being used by another process."
                  << std::endl;
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
    if (_verbose) {
        std::cout << "[LOG] " << message << std::endl;
    }
}

void UDPServer(UdpServer &udpServer, CommandManager &manager, Server &server) {
    while (1) {
        std::stringstream message = udpServer.receive();
        server.showMessage(Message::ServerConnectionDetails(
            udpServer.getClientIP(), udpServer.getClientPort(), "UDP"));
        StreamMessage streamMessage(message);
        std::stringstream response;
        manager.readCommand(streamMessage, response, server, false);
        udpServer.send(response);
    }
}

void TCPServer(TcpServer &tcpServer, CommandManager &manager, Server &server) {
    while (1) {
        struct sockaddr_in client;
        socklen_t clientSize;
        int fd = tcpServer.acceptConnection(client, clientSize);

        TcpSession session(fd, client, clientSize);

        pid_t pid;
        if ((pid = fork()) == -1) {
            exit(1);
        } else if (pid == 0) {
            server.showMessage(Message::ServerConnectionDetails(
                session.getClientIP(), session.getClientPort(), "TCP"));
            TcpMessage message(session._fd);
            std::stringstream response;
            manager.readCommand(message, response, server, true);
            session.send(response);
            exit(0);
        }
    }
}
