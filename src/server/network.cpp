/**
 * @file network.cpp
 * @brief Implementation of the network classes for UDP and TCP servers.
 */
#include "network.hpp"

/**
 * @brief Constructor for the UdpServer class.
 * 
 * @param port The port number to bind the server to.
 */
UdpServer::UdpServer(std::string port) {
    _fd = socket(AF_INET, SOCK_DGRAM, 0); //Create the socket

    if (_fd == -1) {  //Check for errors
        throw SocketException();
    }

    //Set the socket options
    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_DGRAM;
    _hints.ai_flags = AI_PASSIVE;

    int err = getaddrinfo(NULL, port.c_str(), &_hints, &_res); //Get the address info

    if (err != 0) { //Check for errors
        throw SocketException();
    }

    if (bind(_fd, _res->ai_addr, _res->ai_addrlen) == -1) { //Bind the socket
        throw SocketException();
    }
}

/**
 * @brief Destructor for the UdpServer class.
 * 
 * This destructor is responsible for cleaning up any resources used by the UdpServer object.
 */
UdpServer::~UdpServer() {
    if (!_closed) { //Check if the socket has been closed
        ::close(_fd); //Close the socket
    }
    freeaddrinfo(_res); //Free the address info
}

/**
 * @brief Sends a message via UDP.
 *
 * @param message The message to be sent.
 */
void UdpServer::send(std::stringstream &message) {
    char messageBuffer[SOCKETS_MAX_DATAGRAM_SIZE_SERVER]; //The message buffer

    message.read(messageBuffer, SOCKETS_MAX_DATAGRAM_SIZE_SERVER); //Read the message

    std::streamsize n = message.gcount(); //Get the number of bytes read

    if (n <= 0) { //Check for errors
        throw SocketException();
    }

    //Send the message
    if (sendto(_fd, messageBuffer, (size_t)n, 0, (struct sockaddr *)&_client,
               _clientSize) != n) {
        throw SocketException();
    }
}

std::stringstream UdpServer::receive() {
    char messageBuffer[SOCKETS_MAX_DATAGRAM_SIZE_SERVER + 1]; //The message buffer
    ssize_t n =
        recvfrom(_fd, messageBuffer, SOCKETS_MAX_DATAGRAM_SIZE_SERVER + 1, 0,
                 (struct sockaddr *)&_client, &_clientSize); //Receive the message

    if (n > SOCKETS_MAX_DATAGRAM_SIZE_SERVER) { //Check for errors
        throw SocketException();
    }

    std::stringstream message;

    message.write(messageBuffer, (std::streamsize)n); //Write the message to the stringstream

    return message;
}

/**
 * @brief Closes the UDP server.
 */
void UdpServer::close() {
    if (!_closed) { //Check if the socket has been closed
        ::close(_fd); //Close the socket
    }
}

/**
 * @brief Returns the IP address of the client connected to the UDP server.
 *
 * @return The IP address of the client as a string.
 */
std::string UdpServer::getClientIP() {
    char ip[INET_ADDRSTRLEN]; //The IP address buffer
    inet_ntop(AF_INET, &_client.sin_addr, ip, INET_ADDRSTRLEN); //Get the IP address
    return std::string(ip);
}

/**
 * @brief Returns the port number of the client.
 *
 * @return The port number of the client as a string.
 */
std::string UdpServer::getClientPort() {
    return std::to_string(ntohs(_client.sin_port)); //Get the port number
}

/**
 * @brief Constructs a TcpServer object with the specified port.
 *
 * @param port The port number to bind the server to.
 */
TcpServer::TcpServer(std::string port) {
    _fd = socket(AF_INET, SOCK_STREAM, 0); //Create the socket
    if (_fd == -1) { //Check for errors
        throw SocketException();
    }

    int reuse_addr = 1; //Set the socket options

    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

    //Set the socket options
    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_STREAM;
    _hints.ai_flags = AI_PASSIVE;

    int n = getaddrinfo(NULL, port.c_str(), &_hints, &_res); //Get the address info

    if (n != 0) { //Check for errors
        throw SocketException();
    }

    if (bind(_fd, _res->ai_addr, _res->ai_addrlen) == -1) { //Bind the socket
        throw SocketException();
    }

    if (listen(_fd, 20) == -1) { //Listen for connections
        throw SocketException();
    }
}

/**
 * @brief Destructor for the TcpServer class.
 * 
 * This destructor is responsible for cleaning up any resources
 * used by the TcpServer object.
 */
TcpServer::~TcpServer() {
    if (!_closed) { //Check if the socket has been closed
        ::close(_fd); //Close the socket
    }
    freeaddrinfo(_res); //Free the address info
}

/**
 * Accepts a new client connection.
 *
 * @param client The client's socket address structure.
 * @param clientSize The size of the client's socket address structure.
 * @return The file descriptor of the accepted connection, or -1 if an error occurred.
 */
int TcpServer::acceptConnection(struct sockaddr_in &client,
                                socklen_t &clientSize) {
    sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);

    int clientFd =
        accept(_fd, (struct sockaddr *)&clientAddress, &clientAddressSize); //Accept the connection

    client = clientAddress; //Set the client address
    clientSize = clientAddressSize; //Set the client address size

    if (clientFd == -1) { //Check for errors
        throw SocketException();
    }

    return clientFd;
}

/**
 * @brief Closes the TCP server.
 */
void TcpServer::close() {
    if (!_closed) { //Check if the socket has been closed
        ::close(_fd); //Close the socket
    }
}

/**
 * @brief Constructs a TcpSession object.
 *
 * @param fd The file descriptor of the TCP session.
 * @param client The client's socket address structure.
 * @param clientSize The size of the client's socket address structure.
 */
TcpSession::TcpSession(int fd, struct sockaddr_in client,
                       socklen_t clientSize) {
    _fd = fd; //Set the file descriptor
    _client = client; //Set the client address
    _clientSize = clientSize; //Set the client address size

    struct timeval read_timeout; //Set the read timeout
    read_timeout.tv_sec = 5; //Set the timeout to 5 seconds
    read_timeout.tv_usec = 0; 

    setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout,
               sizeof(read_timeout)); //Set the socket options
}

/**
 * @brief Destructor for the TcpSession class.
 */
TcpSession::~TcpSession() {
    if (!_closed) { //Check if the socket has been closed
        ::close(_fd); //Close the socket
    }
}

/**
 * @brief Sends a message over the TCP session.
 *
 * @param message The message to be sent.
 */
void TcpSession::send(std::stringstream &message) {
    char messageBuffer[SOCKETS_TCP_BUFFER_SIZE]; //The message buffer

    message.read(messageBuffer, SOCKETS_TCP_BUFFER_SIZE); //Read the message from the stringstream

    ssize_t n = message.gcount(); //Get the number of bytes read

    while (n != 0) { //Do loop until the end of the message
        if (write(_fd, messageBuffer, (size_t)n) == -1) { //Send the message
            throw SocketException();
        }
        message.read(messageBuffer, SOCKETS_TCP_BUFFER_SIZE); //Read the message
        n = message.gcount(); //Get the number of bytes read
    }
}

/**
 * @brief Receives data from the TCP session.
 * 
 * @return std::stringstream The received data.
 */
std::stringstream TcpSession::receive() {
    char messageBuffer[SOCKETS_TCP_BUFFER_SIZE]; //The message buffer
    std::stringstream message;

    ssize_t n = read(_fd, messageBuffer, SOCKETS_TCP_BUFFER_SIZE); //Read the message from the socket

    if (n == -1) {
        throw SocketException();
    }

    while (n != 0) { //Do loop until the end of the message
        message.write(messageBuffer, n); //Write the message to the stringstream

        n = read(_fd, messageBuffer, SOCKETS_TCP_BUFFER_SIZE); //Read the message from the socket

        if (n == -1) { //Check for errors
            if (errno == EAGAIN || errno == EWOULDBLOCK ||
                errno == EINPROGRESS) {
                break;
            }
            throw SocketException();
        }
    }

    return message;
}

/**
 * @brief Closes the TCP session.
 */
void TcpSession::close() {
    if (!_closed) { //Check if the socket has been closed
        ::close(_fd); //Close the socket
    }
}

/**
 * @brief Returns the IP address of the client connected to the TCP session.
 *
 * @return The IP address of the client as a string.
 */
std::string TcpSession::getClientIP() {
    char ip[INET_ADDRSTRLEN]; //The IP address buffer
    inet_ntop(AF_INET, &_client.sin_addr, ip, INET_ADDRSTRLEN); //Get the IP address
    return std::string(ip);
}

/**
 * @brief Returns the port number of the client connected to the TCP session.
 *
 * @return The client port number as a string.
 */
std::string TcpSession::getClientPort() {
    return std::to_string(ntohs(_client.sin_port)); //Get the port number
}