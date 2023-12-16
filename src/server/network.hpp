/**
 * @file network.hpp
 * @brief Header file for the network program.
 * 
 * This file contains the declaration of the UdpServer, TcpServer, TcpSession, SocketException, and TimeoutException classes.
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
    int _fd;  //The file descriptor created by the socket() function
    bool _closed = false;  //Flag indicating whether the server has been closed
    struct addrinfo _hints;                   //The address info hints
    struct addrinfo *_res;                    //The address info res
    struct sockaddr_in _client;               //The client address
    socklen_t _clientSize = sizeof(_client);  //The client address size

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

    /**
     * @brief Closes the network connection.
     */
    void close();

    /**
     * @brief Get the IP address of the client.
     * 
     * @return std::string The IP address of the client.
     */
    std::string getClientIP();

    /**
     * @brief Returns the port number of the client.
     * 
     * @return std::string The client port number.
     */
    std::string getClientPort();
};

/**
 * @class TcpServer
 * @brief Represents a TCP server that can accept connections over the network.
 */
class TcpServer {
  private:
    int _fd;  //The file descriptor created by the socket() function
    bool _closed = false;  //Flag indicating whether the server has been closed
    struct addrinfo _hints;  //The address info hints
    struct addrinfo *_res;   //The address info res

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
     * @brief Closes the network connection.
     */
    void close();

    /**
     * @brief Accepts incoming connections.
     * 
     * This function is responsible for accepting incoming connections from clients.
     * It should be called in a loop to continuously accept new connections.
     */
    int acceptConnection(struct sockaddr_in &client, socklen_t &clientSize);
};

/**
 * @class TcpSession
 * @brief Represents a TCP session that can send and receive data over the network.
 */
class TcpSession {
  private:
    bool _closed = false;  //Flag indicating whether the session has been closed
    struct sockaddr_in _client;  //The client address
    socklen_t _clientSize;       //The client address size

  public:
    int _fd;
    /**
     * @brief Constructs a TcpSession object with the specified file descriptor, addres info hints and res.
     * @param _fd The file descriptor.
     * @param _hints The address info hints.
     * @param _res The address info res.
     */
    TcpSession(int fd, struct sockaddr_in client, socklen_t clientSize);

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

    /**
     * @brief Closes the network connection.
     */
    void close();

    /**
     * @brief Get the IP address of the client.
     * 
     * @return std::string The IP address of the client.
     */
    std::string getClientIP();

    /**
     * @brief Returns the port number of the client.
     * 
     * @return std::string The client port number.
     */
    std::string getClientPort();
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
 * @class SocketSetupException
 * @brief Exception thrown when there is a problem setting up the sockets.
 */
class SocketSetupException : public SocketException {};

/**
 * @class SocketCommunicationException
 * @brief Exception thrown when there is a problem reading/writing from/to a socket.
 */
class SocketCommunicationException : public SocketException {};

#endif