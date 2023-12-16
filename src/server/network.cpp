/**
 * @file network.cpp
 * @brief Implementation of the network classes for UDP and TCP servers.
 */
#include "network.hpp"

UdpServer::UdpServer(std::string port) {
    _fd = socket(AF_INET, SOCK_DGRAM, 0);  //Create the socket

    if (_fd == -1) {  //Check for errors
        throw SocketSetupException();
    }

    //Set the socket options
    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_DGRAM;
    _hints.ai_flags = AI_PASSIVE;

    int err =
        getaddrinfo(NULL, port.c_str(), &_hints, &_res);  //Get the address info

    if (err != 0) {  //Check for errors
        throw SocketSetupException();
    }

    if (bind(_fd, _res->ai_addr, _res->ai_addrlen) == -1) {  //Bind the socket
        throw SocketSetupException();
    }
}

UdpServer::~UdpServer() {
    if (!_closed) {    //Check if the socket has been closed
        ::close(_fd);  //Close the socket
    }
    freeaddrinfo(_res);  //Free the address info
}

void UdpServer::send(std::stringstream &message) {
    char messageBuffer[SOCKETS_MAX_DATAGRAM_SIZE_SERVER];  //The message buffer

    message.read(messageBuffer,
                 SOCKETS_MAX_DATAGRAM_SIZE_SERVER);  //Read the message

    std::streamsize n = message.gcount();  //Get the number of bytes read

    if (n <= 0) {  //Check for errors
        throw SocketCommunicationException();
    }

    //Send the message
    if (sendto(_fd, messageBuffer, (size_t)n, 0, (struct sockaddr *)&_client,
               _clientSize) != n) {
        throw SocketCommunicationException();
    }
}

std::stringstream UdpServer::receive() {
    char messageBuffer[SOCKETS_MAX_DATAGRAM_SIZE_SERVER +
                       1];  //The message buffer
    ssize_t n = recvfrom(
        _fd, messageBuffer, SOCKETS_MAX_DATAGRAM_SIZE_SERVER + 1, 0,
        (struct sockaddr *)&_client, &_clientSize);  //Receive the message

    if (n > SOCKETS_MAX_DATAGRAM_SIZE_SERVER) {  //Check for errors
        throw SocketCommunicationException();
    }

    std::stringstream message;

    message.write(messageBuffer,
                  (std::streamsize)n);  //Write the message to the stringstream

    return message;
}

void UdpServer::close() {
    if (!_closed) {    //Check if the socket has been closed
        ::close(_fd);  //Close the socket
    }
}

std::string UdpServer::getClientIP() {
    char ip[INET_ADDRSTRLEN];  //The IP address buffer
    inet_ntop(AF_INET, &_client.sin_addr, ip,
              INET_ADDRSTRLEN);  //Get the IP address
    return std::string(ip);
}

std::string UdpServer::getClientPort() {
    return std::to_string(ntohs(_client.sin_port));  //Get the port number
}

TcpServer::TcpServer(std::string port) {
    _fd = socket(AF_INET, SOCK_STREAM, 0);  //Create the socket
    if (_fd == -1) {                        //Check for errors
        throw SocketSetupException();
    }

    int reuse_addr = 1;  //Set the socket options

    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

    //Set the socket options
    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_STREAM;
    _hints.ai_flags = AI_PASSIVE;

    int n =
        getaddrinfo(NULL, port.c_str(), &_hints, &_res);  //Get the address info

    if (n != 0) {  //Check for errors
        throw SocketSetupException();
    }

    if (bind(_fd, _res->ai_addr, _res->ai_addrlen) == -1) {  //Bind the socket
        throw SocketSetupException();
    }

    if (listen(_fd, 20) == -1) {  //Listen for connections
        throw SocketSetupException();
    }
}

TcpServer::~TcpServer() {
    if (!_closed) {    //Check if the socket has been closed
        ::close(_fd);  //Close the socket
    }
    freeaddrinfo(_res);  //Free the address info
}

int TcpServer::acceptConnection(struct sockaddr_in &client,
                                socklen_t &clientSize) {
    sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);

    int clientFd = accept(_fd, (struct sockaddr *)&clientAddress,
                          &clientAddressSize);  //Accept the connection

    client = clientAddress;          //Set the client address
    clientSize = clientAddressSize;  //Set the client address size

    if (clientFd == -1) {  //Check for errors
        throw SocketSetupException();
    }

    return clientFd;
}

void TcpServer::close() {
    if (!_closed) {    //Check if the socket has been closed
        ::close(_fd);  //Close the socket
    }
}

TcpSession::TcpSession(int fd, struct sockaddr_in client,
                       socklen_t clientSize) {
    _fd = fd;                  //Set the file descriptor
    _client = client;          //Set the client address
    _clientSize = clientSize;  //Set the client address size

    struct timeval read_timeout;  //Set the read timeout
    read_timeout.tv_sec = 5;      //Set the timeout to 5 seconds
    read_timeout.tv_usec = 0;

    setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout,
               sizeof(read_timeout));  //Set the socket options
}

TcpSession::~TcpSession() {
    if (!_closed) {    //Check if the socket has been closed
        ::close(_fd);  //Close the socket
    }
}

void TcpSession::send(std::stringstream &message) {
    char messageBuffer[SOCKETS_TCP_BUFFER_SIZE];  //The message buffer

    message.read(
        messageBuffer,
        SOCKETS_TCP_BUFFER_SIZE);  //Read the message from the stringstream

    ssize_t n = message.gcount();  //Get the number of bytes read

    while (n != 0) {  //Do loop until the end of the message
        if (write(_fd, messageBuffer, (size_t)n) == -1) {  //Send the message
            throw SocketCommunicationException();
        }
        message.read(messageBuffer,
                     SOCKETS_TCP_BUFFER_SIZE);  //Read the message
        n = message.gcount();                   //Get the number of bytes read
    }
}

void TcpSession::close() {
    if (!_closed) {    //Check if the socket has been closed
        ::close(_fd);  //Close the socket
    }
}

std::string TcpSession::getClientIP() {
    char ip[INET_ADDRSTRLEN];  //The IP address buffer
    inet_ntop(AF_INET, &_client.sin_addr, ip,
              INET_ADDRSTRLEN);  //Get the IP address
    return std::string(ip);
}

std::string TcpSession::getClientPort() {
    return std::to_string(ntohs(_client.sin_port));  //Get the port number
}