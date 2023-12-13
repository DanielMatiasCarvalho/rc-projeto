/**
 * @file server.cpp
 * @brief Implementation file for the server program.
 * 
 * This file contains the implementation of the main function and other functions
 * related to starting and running the UDP and TCP servers.
 */
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

/**
 * @brief The main function of the server program.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 * @return An integer representing the exit status of the program.
 */
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
    } catch (SocketException const
                 &e) {  //If the server could not connect to the sockets
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

    _database = std::make_unique<Database>(
        "database");  //Initialize the database in path ./database
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

/**
 * @brief Function that handles the UDP server.
 * 
 * This function receives an instance of UdpServer, CommandManager, and Server classes
 * and performs the necessary operations to handle the UDP server requests.
 * 
 * @param udpServer The UDP server instance.
 * @param manager The command manager instance.
 * @param server The server instance.
 */
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

/**
 * @brief Function that handles the TCP server.
 * 
 * This function receives an instance of TcpServer, CommandManager, and Server classes
 * and performs the necessary operations to handle the TCP server requests.
 * 
 * @param tcpServer The TCP server instance.
 * @param manager The command manager instance.
 * @param server The server instance.
 */
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
            tcpServer.close();  //Close the TCP server
            server.showMessage(
                Message::
                    ServerConnectionDetails(  //Display the client information if verbose mode is enabled
                        session.getClientIP(), session.getClientPort(), "TCP"));
            TcpMessage message(session._fd);  //Initialize the TCP message
            std::stringstream response;       //Initialize the response stream
            manager.readCommand(
                message, response, server,
                true);  //Read the command, handle it and write the response
            session.send(response);  //Send the response to the client
            exit(0);                 //Exit the child process
        }
    }
}
