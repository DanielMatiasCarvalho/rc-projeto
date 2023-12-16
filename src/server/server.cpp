/**
 * @file server.cpp
 * @brief Implementation file for the server program.
 * 
 * This file contains the implementation of the main function and other functions
 * related to starting and running the UDP and TCP servers.
 */
#include "server.hpp"
#include "command.hpp"

void UDPServer(UdpServer &udpServer, CommandManager &manager, Server &server);

void TCPServer(TcpServer &tcpServer, CommandManager &manager, Server &server);

int main(int argc, char **argv) {
    Server server(argc, argv);  //Initialize the server
    CommandManager manager;     //Initialize the command manager
    struct sigaction act;       //Initialize the signal handler
    pid_t pid;                  //Initialize the process ID

    act.sa_handler = SIG_IGN;  //Ignore the signal
    if (sigaction(SIGCHLD, &act, NULL) ==
        -1) {  //Ignore the signal of the child process death
        exit(1);
    }
    if (sigaction(SIGPIPE, &act, NULL) ==
        -1) {  //Ignore the signal of the broken pipe
        exit(1);
    }

    //Register the commands handlers
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

    try {                                       //Try to start the server
        UdpServer udpServer(server.getPort());  //Initialize the UDP server
        TcpServer tcpServer(server.getPort());  //Initialize the TCP server

        //Display the server information if verbose mode is enabled
        server.showMessage("Listening on port " + server.getPort());

        if ((pid = fork()) == -1) {  //Fork the process
            exit(1);
        } else if (pid == 0) {  //If the process is a child process
            tcpServer.close();  //Close the TCP server
            server.showMessage(
                "UDP server started");  //Display a message if verbose mode is enabled
            UDPServer(udpServer, manager, server);  //Start the UDP server
        } else {                //If the process is a parent process
            udpServer.close();  //Close the UDP server
            server.showMessage(
                "TCP server started");  //Display a message if verbose mode is enabled
            TCPServer(tcpServer, manager, server);  //Start the TCP server
        }
    } catch (SocketSetupException const
                 &e) {  //If the server could not connect to the sockets
        std::cout << "Server could not connect to the sockets. Ensure that the "
                     "port is not being used by another process."
                  << std::endl;
    }

    return 0;
}

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

    _database = std::make_unique<Database>(
        "database");  //Initialize the database in path ./database
}

void Server::ShowInfo() {
    std::cout << "Port: " << _port << std::endl
              << "Verbose: " << _verbose << std::endl;
}

void Server::showMessage(std::string message) {
    if (_verbose) {
        std::cout << "[LOG] " << message << std::endl;
    }
}

void UDPServer(UdpServer &udpServer, CommandManager &manager, Server &server) {
    while (1) {
        std::stringstream message =
            udpServer.receive();  //Receive a message from the client
        server.showMessage(
            Message::
                ServerConnectionDetails(  //Display the client information if verbose mode is enabled
                    udpServer.getClientIP(), udpServer.getClientPort(), "UDP"));
        StreamMessage streamMessage(message);  //Initialize the stream message
        std::stringstream response;            //Initialize the response stream
        manager.readCommand(
            streamMessage, response, server,
            false);  //Read the command, handle it and write the response
        udpServer.send(response);  //Send the response to the client
    }
}

void TCPServer(TcpServer &tcpServer, CommandManager &manager, Server &server) {
    while (1) {
        struct sockaddr_in client;
        socklen_t clientSize;
        //Accept a connection from the client, storing its information in client and clientSize
        int fd = tcpServer.acceptConnection(client, clientSize);

        TcpSession session(fd, client,
                           clientSize);  //Initialize the TCP session

        pid_t pid;
        if ((pid = fork()) == -1) {  //Fork the process
            exit(1);
        } else if (pid == 0) {  //If the process is a child process
            tcpServer.close();  //Close the TCP server socket
            server.showMessage(
                Message::
                    ServerConnectionDetails(  //Display the client information if verbose mode is enabled
                        session.getClientIP(), session.getClientPort(), "TCP"));

            try {
                TcpMessage message(session._fd);  //Initialize the TCP message
                std::stringstream response;       //Initialize the response stream
                manager.readCommand(
                    message, response, server,
                    true);  //Read the command, handle it and write the response
                session.send(response);  //Send the response to the client
            } catch (SocketCommunicationException const &e) {
                server.showMessage("Session ended prematurely.");
            }
            
            exit(0);                 //Exit the child process
        }
    }
}
