/**
 * @file server.cpp
 * @brief Implementation file for the server program.
 *
 * This file contains the implementation of the main function and other
 * functions related to starting and running the UDP and TCP servers.
 */
#include "server.hpp"
#include "command.hpp"

void UDPServer(UdpServer &udpServer, CommandManager &manager, Server &server);

void TCPServer(TcpServer &tcpServer, CommandManager &manager, Server &server);

void handler(int sig) {
    // This is a handler used for the SIGINT command, it cannot be SIG_IGN
    // because with that handler, blocking functions don't stop. With this
    // handler, blocking functions will be interrupted throwing an exeption.
    (void)sig;
}

int main(int argc, char **argv) {
    Server server(argc, argv);   // Initialize the server
    CommandManager manager;      // Initialize the command manager
    struct sigaction act, act2;  // Initialize the signal handler
    pid_t pid;                   // Initialize the process ID

    act.sa_handler = SIG_IGN;  // Ignore the signal
    if (sigaction(SIGCHLD, &act, NULL) ==
        -1) {  // Ignore the signal of the child process death
        exit(1);
    }
    if (sigaction(SIGPIPE, &act, NULL) ==
        -1) {  // Ignore the signal of the broken pipe
        exit(1);
    }

    act2.sa_handler = handler;
    if (sigaction(SIGINT, &act2, NULL) == -1) {
        exit(1);
    }

    // Register the commands handlers
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

    try {                                       // Try to start the server
        UdpServer udpServer(server.getPort());  // Initialize the UDP server
        TcpServer tcpServer(server.getPort());  // Initialize the TCP server

        // Display the server information if verbose mode is enabled
        server.logPush("Listening on port " + server.getPort());
        if ((pid = fork()) == -1) {  // Fork the process
            exit(1);
        } else if (pid == 0) {  // If the process is a child process
            tcpServer.close();  // Close the TCP server
            server.logPush("UDP server started");   // Display a message if
                                                    // verbose mode is enabled
            UDPServer(udpServer, manager, server);  // Start the UDP server
        } else {                // If the process is a parent process
            udpServer.close();  // Close the UDP server
            server.logPush("TCP server started");   // Display a message if
                                                    // verbose mode is enabled
            TCPServer(tcpServer, manager, server);  // Start the TCP server
        }
    } catch (SocketSetupException const
                 &e) {  // If the server could not connect to the sockets
        std::cout << "Server could not connect to the sockets. Ensure that the "
                     "port is not being used by another process."
                  << std::endl;
    } catch (SocketCommunicationException const &e) {
        // We catch this exception because it will be thrown by functions that
        // block (accept and recvfrom) when there is a signal that is not
        // ignored (in this case SIGINT).
    }

    return 0;
}

Server::Server(int argc, char **argv) {
    char c;
    // Parse command line arguments
    while ((c = (char)getopt(argc, argv, "p:v")) != -1) {
        switch (c) {
            case 'p':  // Sets the port
                _port = optarg;
                break;
            case 'v':  // Sets the verbosity
                _loggers.push_back(std::make_shared<Logger>());
                break;
            default:
                break;
        }
    }

    _database = std::make_unique<Database>(
        "database");  // Initialize the database in path ./database
}

void Server::ShowInfo() {
    std::cout << "Port: " << _port << std::endl
              << "Verbose: " << _verbose << std::endl;
}

void Server::log(std::string message) {
    for (auto x : _loggers) {
        x->log(message);
    }
}

void Server::push() {
    for (auto x : _loggers) {
        x->push();
    }
}

void Server::logPush(std::string message) {
    for (auto x : _loggers) {
        x->logPush(message);
    }
}

void UDPServer(UdpServer &udpServer, CommandManager &manager, Server &server) {
    while (1) {
        std::stringstream message =
            udpServer.receive();  // Receive a message from the client
        server.log(
            Message::ServerConnectionDetails(  // Display the client information
                                               // if verbose mode is enabled
                udpServer.getClientIP(), udpServer.getClientPort(), "UDP"));
        StreamMessage streamMessage(message);  // Initialize the stream message
        std::stringstream response;            // Initialize the response stream
        manager.readCommand(
            streamMessage, response, server,
            false);  // Read the command, handle it and write the response
        udpServer.send(response);  // Send the response to the client
        server.push();
    }
}

void TCPServer(TcpServer &tcpServer, CommandManager &manager, Server &server) {
    while (1) {
        struct sockaddr_in client;
        socklen_t clientSize;
        // Accept a connection from the client, storing its information in
        // client and clientSize
        int fd = tcpServer.acceptConnection(client, clientSize);

        TcpSession session(fd, client,
                           clientSize);  // Initialize the TCP session

        pid_t pid;
        if ((pid = fork()) == -1) {  // Fork the process
            exit(1);
        } else if (pid == 0) {  // If the process is a child process
            tcpServer.close();  // Close the TCP server socket
            server.log(
                Message::ServerConnectionDetails(  // Display the client
                                                   // information if verbose
                                                   // mode is enabled
                    session.getClientIP(), session.getClientPort(), "TCP"));

            try {
                TcpMessage message(session._fd);  // Initialize the TCP message
                std::stringstream response;  // Initialize the response stream
                manager.readCommand(message, response, server,
                                    true);  // Read the command, handle it and
                                            // write the response
                session.send(response);     // Send the response to the client
            } catch (SocketCommunicationException const &e) {
                server.log("Session ended prematurely.");
            }
            server.push();
            exit(0);  // Exit the child process
        }
    }
}

void Logger::log(std::string message) {
    _messages.push_back(message);
}

void Logger::push() {
    time_t currentTime = std::time(nullptr);
    std::tm *currentTm = std::localtime(&currentTime);
    for (auto message : _messages) {
        std::cout << "[" << std::put_time(currentTm, "%Y-%m-%d %H:%M:%S")
                  << "] [LOG] " << message << '\n';
    }

    std::cout << std::flush;

    _messages.clear();
}

void Logger::logPush(std::string message) {
    log(message);
    push();
}