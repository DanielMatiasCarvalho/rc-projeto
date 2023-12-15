/**
 * @file network.hpp
 * @brief This file contains the declaration of the UdpClient, TcpClient, SocketException, and TimeoutException classes.
 */

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

/**
 * @class UdpClient
 * @brief Represents a UDP client that can send and receive data over the network.
 */
class UdpClient {
  private:
    int _fd;                   //The file descriptor of the socket
    struct addrinfo _hints;    //The address flags
    struct addrinfo *_res;     //The address info
    struct sockaddr_in _addr;  //The address

  public:
    /**
     * @brief Constructs a UdpClient object with the specified hostname and port.
     * @param hostname The hostname or IP address of the server.
     * @param port The port number to connect to.
     */
    UdpClient(std::string hostname, std::string port);

    /**
     * @brief Destroys the UdpClient object and closes the socket.
     */
    ~UdpClient();

    /**
     * @brief Sends the specified message to the server.
     * @param message The message to send.
     */
    void send(std::stringstream &message);

    /**
     * @brief Receives a message from the server.
     * @return The received message as a stringstream.
     */
    std::stringstream receive();
};

/**
 * @class TcpClient
 * @brief Represents a TCP client that can send and receive data over the network.
 */
class TcpClient {
  private:
    int _fd;                 //The file descriptor of the socket
    struct addrinfo _hints;  //The address flags
    struct addrinfo *_res;   //The address info

  public:
    /**
     * @brief Constructs a TcpClient object with the specified hostname and port.
     * @param hostname The hostname or IP address of the server.
     * @param port The port number to connect to.
     */
    TcpClient(std::string hostname, std::string port);

    /**
     * @brief Destroys the TcpClient object and closes the socket.
     */
    ~TcpClient();

    /**
     * @brief Sends the specified message to the server.
     * @param message The message to send.
     */
    void send(std::stringstream &message);

    /**
     * @brief Receives a message from the server.
     * @return The received message as a stringstream.
     */
    std::stringstream receive();
};

/**
 * @class SocketException
 * @brief Represents an exception that is thrown when a network error occurs.
 */
class SocketException : public std::runtime_error {
  public:
    /**
     * @brief Constructs a SocketException object.
     */
    SocketException() : std::runtime_error("A network error has occurred."){};
};

/**
 * @class TimeoutException
 * @brief Represents an exception that is thrown when the server has timed out.
 */
class TimeoutException : public std::runtime_error {
  public:
    /**
     * @brief Constructs a TimeoutException object.
     */
    TimeoutException() : std::runtime_error("Server has timed out."){};
};

#endif