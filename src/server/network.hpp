/**
 * @file network.hpp
 * @brief This file contains the declaration of the UdpServer, TcpServer, SocketException, and TimeoutException classes.
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
 * @class UdpServer
 * @brief Represents a UDP server that can send and receive data over the network.
 */
class UdpServer {
  private:
    int _fd;
    struct addrinfo _hints;
    struct addrinfo *_res;
    struct sockaddr_in *_client;
    socklen_t _clientSize = sizeof(_client);

  public:
    /**
     * @brief Constructs a UdpServer object with the specified port.
     * @param port The port number to connect to.
     */
    UdpServer(std::string port);

    /**
     * @brief Destroys the UdpServer object and closes the socket.
     */
    ~UdpServer();

    /**
     * @brief Sends the specified message to the client.
     * @param message The message to send.
     */
    void send(std::stringstream &message);

    /**
     * @brief Receives a message from the client.
     * @return The received message as a stringstream.
     */
    std::stringstream receive();
};

/**
 * @class TcpServer
 * @brief Represents a TCP server that can accept connections over the network.
 */
class TcpServer {
  private:
    int _fd;
    struct addrinfo _hints;
    struct addrinfo *_res;

  public:
    /**
     * @brief Constructs a TcpServer object with the specified port.
     * @param port The port number to connect to.
     */
    TcpServer(std::string port);

    /**
     * @brief Destroys the TcpServer object and closes the socket.
     */
    ~TcpServer();

    /**
     * @brief Accepts incoming connections.
     * 
     * This function is responsible for accepting incoming connections from clients.
     * It should be called in a loop to continuously accept new connections.
     */
    int acceptConnection();
};

/**
 * @class TcpSession
 * @brief Represents a TCP session that can send and receive data over the network.
 */
class TcpSession {
  private:
    int _fd;

  public:
    /**
     * @brief Constructs a TcpSession object with the specified file descriptor, addres info hints and res.
     * @param _fd The file descriptor.
     * @param _hints The address info hints.
     * @param _res The address info res.
     */
    TcpSession(int fd) : _fd(fd){};

    /**
     * @brief Destroys the TcpSession object and closes the socket.
     */
    ~TcpSession();

    /**
     * @brief Sends the specified message to the client.
     * @param message The message to send.
     */
    void send(std::stringstream &message);

    /**
     * @brief Receives a message from the client.
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