#include "network.hpp"

UdpClient::UdpClient(std::string hostname, std::string port) {
    _fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (_fd == -1) {
        throw SocketException();
    }

    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_DGRAM;

    int err = getaddrinfo(hostname.c_str(), port.c_str(), &_hints, &_res);

    if (err != 0) {
        throw SocketException();
    }

    struct timeval timeout;
    timeout.tv_sec = SOCKETS_UDP_TIMEOUT;
    timeout.tv_usec = 0;

    if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) <
        0) {
        throw SocketException();
    }
}

UdpClient::~UdpClient() {
    freeaddrinfo(_res);
    close(_fd);
}

void UdpClient::send(std::stringstream &message) {
    char messageBuffer[SOCKETS_MAX_DATAGRAM_SIZE];

    message.read(messageBuffer, SOCKETS_MAX_DATAGRAM_SIZE);

    std::streamsize n = message.gcount();

    if (n <= 0) {
        throw SocketException();
    }

    if (sendto(_fd, messageBuffer, (size_t)n, 0, _res->ai_addr,
               _res->ai_addrlen) != n) {
        throw SocketException();
    }
}

std::stringstream UdpClient::receive() {
    char messageBuffer[SOCKETS_MAX_DATAGRAM_SIZE + 1];
    socklen_t addrlen = sizeof(_addr);
    ssize_t n = recvfrom(_fd, messageBuffer, SOCKETS_MAX_DATAGRAM_SIZE + 1, 0,
                         (struct sockaddr *)&_addr, &addrlen);

    if (n == -1) {
        throw TimeoutException();
    }
    if (n > SOCKETS_MAX_DATAGRAM_SIZE) {
        throw SocketException();
    }

    std::stringstream message;

    message.write(messageBuffer, (std::streamsize)n);

    return message;
}

TcpClient::TcpClient(std::string hostname, std::string port) {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd == -1) {
        throw SocketException();
    }

    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET;
    _hints.ai_socktype = SOCK_STREAM;

    int n = getaddrinfo(hostname.c_str(), port.c_str(), &_hints, &_res);

    if (n != 0) {
        throw SocketException();
    }

    n = connect(_fd, _res->ai_addr, _res->ai_addrlen);

    if (n == -1) {
        throw TimeoutException();
    }
}

TcpClient::~TcpClient() {
    freeaddrinfo(_res);
    close(_fd);
}

void TcpClient::send(std::stringstream &message) {
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

std::stringstream TcpClient::receive() {
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