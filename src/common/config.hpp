/**
 * @file config.hpp
 * @brief Contains configuration constants for the project.
 */
#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#define PROTOCOL_MESSAGE_DELIMITER \
    '\n'  // The delimiter used to separate messages in the protocol.
#define PROTOCOL_UID_SIZE (6)  // The size of the UID (User ID) in the protocol.
#define PROTOCOL_PASSWORD_SIZE (8)  // The size of the password in the protocol.
#define PROTOCOL_AID_SIZE \
    (3)  // The size of the AID (Action ID) in the protocol.
#define PROTOCOL_ERROR_IDENTIFIER \
    "ERR"  // The identifier used to indicate an error in the protocol.
#define PROTOCOL_MAX_FILE_SIZE \
    10000000  // The maximum size of a file in the protocol.

#define DEFAULT_HOSTNAME \
    "127.0.0.1"               // The default hostname for network connections.
#define DEFAULT_PORT "58085"  // The default port for network connections.

#define SOCKETS_MAX_DATAGRAM_SIZE_CLIENT \
    (6001)  // The maximum size of a datagram for a client socket.
#define SOCKETS_MAX_DATAGRAM_SIZE_SERVER \
    (20)  // The maximum size of a datagram for a server socket.
#define SOCKETS_TCP_BUFFER_SIZE \
    (512)  // The size of the TCP buffer for socket communication.
#define SOCKETS_UDP_TIMEOUT \
    (5)  // The timeout value for UDP socket communication.

#endif