#include "network.hpp"

UdpServer::UdpServer(std::string port) {
    _fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (_fd == -1) {
        throw SocketException();
    }

    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_DGRAM;
    _hints.ai_flags = AI_PASSIVE;

    int err = getaddrinfo(NULL, port.c_str(), &_hints, &_res);

    if (err != 0) {
        throw SocketException();
    }

    if (bind(_fd, _res->ai_addr, _res->ai_addrlen) == -1) {
        throw SocketException();
    }
}

UdpServer::~UdpServer() {
    freeaddrinfo(_res);
    close(_fd);
}

void UdpServer::send(std::stringstream &message) {
    char messageBuffer[SOCKETS_MAX_DATAGRAM_SIZE_SERVER];

    message.read(messageBuffer, SOCKETS_MAX_DATAGRAM_SIZE_SERVER);

    std::streamsize n = message.gcount();

    if (n <= 0) {
        throw SocketException();
    }

    if (sendto(_fd, messageBuffer, (size_t)n, 0, _client->ai_addr,
               _client->ai_addrlen) != n) {
        throw SocketException();
    }
}

std::stringstream UdpServer::receive() {
    char messageBuffer[SOCKETS_MAX_DATAGRAM_SIZE_SERVER + 1];
    ssize_t n =
        recvfrom(_fd, messageBuffer, SOCKETS_MAX_DATAGRAM_SIZE_SERVER + 1, 0,
                 _client->ai_addr, &(_client->ai_addrlen));

    if (n > SOCKETS_MAX_DATAGRAM_SIZE_SERVER) {
        throw SocketException();
    }

    std::stringstream message;

    message.write(messageBuffer, (std::streamsize)n);

    return message;
}

TcpServer::TcpServer(std::string port) {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd == -1) {
        throw SocketException();
    }

    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_STREAM;
    _hints.ai_flags = AI_PASSIVE;

    int n = getaddrinfo(NULL, port.c_str(), &_hints, &_res);

    if (n != 0) {
        throw SocketException();
    }

    if (bind(_fd, _res->ai_addr, _res->ai_addrlen) == -1) {
        throw SocketException();
    }

    if (listen(_fd, 20) == -1) {
        throw SocketException();
    }
}

TcpServer::~TcpServer() {
    freeaddrinfo(_res);
    close(_fd);
}

int TcpServer::acceptConnection() {
    int clientFd = accept(_fd, NULL, NULL);

    if (clientFd == -1) {
        throw SocketException();
    }

    return clientFd;
}

void TcpSession::send(std::stringstream &message) {
    char messageBuffer[SOCKETS_TCP_BUFFER_SIZE];

    message.read(messageBuffer, SOCKETS_TCP_BUFFER_SIZE);

    ssize_t n = message.gcount();

    while (n != 0) {
        if (write(_fd, messageBuffer, (size_t)n) == -1) {
            throw SocketException();
        }
        message.read(messageBuffer, SOCKETS_TCP_BUFFER_SIZE);
        n = message.gcount();
    }
}

std::stringstream TcpSession::receive() {
    char messageBuffer[SOCKETS_TCP_BUFFER_SIZE];
    std::stringstream message;

    ssize_t n = read(_fd, messageBuffer, SOCKETS_TCP_BUFFER_SIZE);

    if (n == -1) {
        throw SocketException();
    }

    while (n != 0) {
        message.write(messageBuffer, n);
        n = read(_fd, messageBuffer, SOCKETS_TCP_BUFFER_SIZE);

        if (n == -1) {
            throw SocketException();
        }
    }

    return message;
}

TcpSession::~TcpSession() {
    close(_fd);
}