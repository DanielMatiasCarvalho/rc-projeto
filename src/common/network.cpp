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
    char messageBuffer[SOCKETS_MAX_DATAGRAM_SIZE];
    socklen_t addrlen = sizeof(_addr);
    ssize_t n = recvfrom(_fd, messageBuffer, SOCKETS_MAX_DATAGRAM_SIZE, 0,
                         (struct sockaddr *)&_addr, &addrlen);

    if (n == -1) {
        throw SocketException();
    }

    std::stringstream message;

    message.write(messageBuffer, (std::streamsize)n);

    return message;
}