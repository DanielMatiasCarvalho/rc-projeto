#ifndef __NETWORK_HPP__
#define __NETWORK_HPP__

#include <sstream>
#include <string>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

#include "config.hpp"

class UdpClient {
  private:
    int _fd;
    struct addrinfo _hints;
    struct addrinfo* _res;
    struct sockaddr_in _addr;

  public:
    UdpClient(std::string hostname, std::string port);
    ~UdpClient();

    void send(std::stringstream& message);
    std::stringstream receive();
};

class TcpClient {
  private:
    int _fd;
    struct addrinfo _hints;
    struct addrinfo* _res;

  public:
    TcpClient(std::string hostname, std::string port);
    ~TcpClient();

    void send(std::stringstream& message);
    std::stringstream receive();
};

class SocketException : public std::runtime_error {
  public:
    SocketException() : std::runtime_error("A network error has occured."){};
};

#endif