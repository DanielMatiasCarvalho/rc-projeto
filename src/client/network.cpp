/**
 * @file network.cpp
 * @brief Implementation of the UdpClient and TcpClient classes for network communication.
 */

#include "network.hpp"

UdpClient::UdpClient(std::string hostname, std::string port) {
    _fd = socket(AF_INET, SOCK_DGRAM, 0); //Create a UDP socket

    if (_fd == -1) {
        throw SocketException();
    }

    //Set the flags of the socket
    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_DGRAM;

    int err = getaddrinfo(hostname.c_str(), port.c_str(), &_hints, &_res); //Get the address info

    if (err != 0) {
        throw SocketException();
    }

    //Set the timeout of the socket
    struct timeval timeout;
    timeout.tv_sec = SOCKETS_UDP_TIMEOUT;
    timeout.tv_usec = 0;

    //Set the timeout
    if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) <
        0) {
        throw SocketException();
    }
}

UdpClient::~UdpClient() {
    freeaddrinfo(_res); //Free the address info
    close(_fd); //Close the socket
}

void UdpClient::send(std::stringstream &message) {
    char messageBuffer[SOCKETS_MAX_DATAGRAM_SIZE_CLIENT];

    message.read(messageBuffer, SOCKETS_MAX_DATAGRAM_SIZE_CLIENT); //Read the message

    std::streamsize n = message.gcount(); //Get the size of the message

    if (n <= 0) {
        throw SocketException();
    }

    if (sendto(_fd, messageBuffer, (size_t)n, 0, _res->ai_addr,
               _res->ai_addrlen) != n) { //Send the message
        throw SocketException();
    }
}

std::stringstream UdpClient::receive() {
    char messageBuffer[SOCKETS_MAX_DATAGRAM_SIZE_CLIENT + 1]; //Create a buffer to store the message, size+1 to check if the server sent a message bigger than it is supposed
    socklen_t addrlen = sizeof(_addr); //Get the size of the address
    ssize_t n =
        recvfrom(_fd, messageBuffer, SOCKETS_MAX_DATAGRAM_SIZE_CLIENT + 1, 0,
                 (struct sockaddr *)&_addr, &addrlen); //Receive the message

    if (n == -1) {
        throw TimeoutException();
    }
    if (n > SOCKETS_MAX_DATAGRAM_SIZE_CLIENT) {
        throw SocketException();
    }

    std::stringstream message;

    message.write(messageBuffer, (std::streamsize)n); //Write the message to a stringstream

    return message;
}

TcpClient::TcpClient(std::string hostname, std::string port) {
    _fd = socket(AF_INET, SOCK_STREAM, 0); //Create a TCP socket
    if (_fd == -1) {
        throw SocketException();
    }

    //Set the flags of the socket
    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_STREAM;

    int n = getaddrinfo(hostname.c_str(), port.c_str(), &_hints, &_res); //Get the address info

    if (n != 0) {
        throw SocketException();
    }

    n = connect(_fd, _res->ai_addr, _res->ai_addrlen); //Connect to the server

    if (n == -1) {
        throw TimeoutException();
    }
}

TcpClient::~TcpClient() {
    freeaddrinfo(_res); //Free the address info
    close(_fd); //Close the socket
}

void TcpClient::send(std::stringstream &message) {
    char messageBuffer[SOCKETS_TCP_BUFFER_SIZE];

    message.read(messageBuffer, SOCKETS_TCP_BUFFER_SIZE); //Read the message

    ssize_t n = message.gcount();

    while (n != 0) { //While there is content to be read, loops and writes to the socket
        if (write(_fd, messageBuffer, (size_t)n) == -1) {
            throw SocketException();
        }
        message.read(messageBuffer, SOCKETS_TCP_BUFFER_SIZE);
        n = message.gcount();
    }
}

std::stringstream TcpClient::receive() {
    char messageBuffer[SOCKETS_TCP_BUFFER_SIZE];
    std::stringstream message;

    ssize_t n = read(_fd, messageBuffer, SOCKETS_TCP_BUFFER_SIZE); //Read the message

    if (n == -1) {
        throw SocketException();
    }

    while (n != 0) { //While there is content to be read, loops and writes to the stringstream
        message.write(messageBuffer, n);

        n = read(_fd, messageBuffer, SOCKETS_TCP_BUFFER_SIZE);

        if (n == -1) {
            throw SocketException();
        }
    }

    return message;
}