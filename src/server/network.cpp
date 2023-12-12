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

    if (sendto(_fd, messageBuffer, (size_t)n, 0, (struct sockaddr *)&_client,
               _clientSize) != n) {
        throw SocketException();
    }
}

std::stringstream UdpServer::receive() {
    char messageBuffer[SOCKETS_MAX_DATAGRAM_SIZE_SERVER + 1];
    ssize_t n =
        recvfrom(_fd, messageBuffer, SOCKETS_MAX_DATAGRAM_SIZE_SERVER + 1, 0,
                 (struct sockaddr *)&_client, &_clientSize);

    if (n > SOCKETS_MAX_DATAGRAM_SIZE_SERVER) {
        throw SocketException();
    }

    std::stringstream message;

    message.write(messageBuffer, (std::streamsize)n);

    return message;
}

std::string UdpServer::getClientIP() {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &_client.sin_addr, ip, INET_ADDRSTRLEN);
    return std::string(ip);
}

std::string UdpServer::getClientPort() {
    return std::to_string(ntohs(_client.sin_port));
}

TcpServer::TcpServer(std::string port) {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd == -1) {
        throw SocketException();
    }

    int reuse_addr = 1;

    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

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

int TcpServer::acceptConnection(struct sockaddr_in &client,
                                socklen_t &clientSize) {
    sockaddr_in clientAddress;
    socklen_t clientAddressSize;

    int clientFd =
        accept(_fd, (struct sockaddr *)&clientAddress, &clientAddressSize);

    client = clientAddress;
    clientSize = clientAddressSize;

    if (clientFd == -1) {
        throw SocketException();
    }

    return clientFd;
}

TcpSession::TcpSession(int fd, struct sockaddr_in client,
                       socklen_t clientSize) {
    _fd = fd;
    _client = client;
    _clientSize = clientSize;

    struct timeval read_timeout;
    read_timeout.tv_sec = 5;
    read_timeout.tv_usec = 0;

    setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout,
               sizeof(read_timeout));
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
            if (errno == EAGAIN || errno == EWOULDBLOCK ||
                errno == EINPROGRESS) {
                break;
            }
            throw SocketException();
        }
    }

    return message;
}

TcpSession::~TcpSession() {
    close(_fd);
}

std::string TcpSession::getClientIP() {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &_client.sin_addr, ip, INET_ADDRSTRLEN);
    return std::string(ip);
}

std::string TcpSession::getClientPort() {
    return std::to_string(ntohs(_client.sin_port));
}