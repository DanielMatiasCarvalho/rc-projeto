#ifndef __PROTOCOL_HPP__
#define __PROTOCOL_HPP__

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <ctime>

#include "config.hpp"
#include "utils.hpp"

class ProtocolException : public std::runtime_error {
  public:
    ProtocolException()
        : std::runtime_error(
              "There was an error while communicating with the server."){};
};

class ProtocolViolationException : public ProtocolException {};

class ProtocolMessageErrorException : public ProtocolException {};

class ProtocolCommunication {
  public:
    // Each subclass should implement their information as members.

    // Methods that encode/decode the message content into/from a string stream.
    virtual std::stringstream encodeRequest() = 0;
    virtual void decodeRequest(std::stringstream &message) = 0;
    virtual std::stringstream encodeResponse() = 0;
    virtual void decodeResponse(std::stringstream &message) = 0;

    // General purpose methods to allow parsing and encoding.
    char readChar(std::stringstream &message);
    void readChar(std::stringstream &message, char expected);
    char readChar(std::stringstream &message, std::vector<char> options);
    void readDelimiter(std::stringstream &message);
    void readSpace(std::stringstream &message);
    std::string readString(std::stringstream &message);
    std::string readString(std::stringstream &message, size_t n);
    void readString(std::stringstream &message, std::string expected);
    std::string readString(std::stringstream &message,
                           std::vector<std::string> options);
    int readNumber(std::stringstream &message);

    void writeChar(std::stringstream &message, char c);
    void writeDelimiter(std::stringstream &message);
    void writeSpace(std::stringstream &message);
    void writeString(std::stringstream &message, std::string string);
    std::time_t readDateTime(std::stringstream &message);
};

class LoginCommunication : ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;
    std::string _password;

    // Response parameters:
    std::string _status;

    std::stringstream encodeRequest();
    void decodeRequest(std::stringstream &message);
    std::stringstream encodeResponse();
    void decodeResponse(std::stringstream &message);
};

class LogoutCommunication : ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;
    std::string _password;

    // Response parameters:
    std::string _status;

    std::stringstream encodeRequest();
    void decodeRequest(std::stringstream &message);
    std::stringstream encodeResponse();
    void decodeResponse(std::stringstream &message);
};

class UnregisterCommunication : ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;
    std::string _password;

    // Response parameters:
    std::string _status;

    std::stringstream encodeRequest();
    void decodeRequest(std::stringstream &message);
    std::stringstream encodeResponse();
    void decodeResponse(std::stringstream &message);
};

class ListUserAuctionsCommunication : ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;

    // Response parameters:
    std::string _status;
    std::unordered_map<std::string, std::string> _auctions;

    std::stringstream encodeRequest();
    void decodeRequest(std::stringstream &message);
    std::stringstream encodeResponse();
    void decodeResponse(std::stringstream &message);
};

class ShowRecordCommunication : ProtocolCommunication {
  public:
    // Request parameters:

    // Response parameters:

    std::stringstream encodeRequest();
    void decodeRequest(std::stringstream &message);
    std::stringstream encodeResponse();
    void decodeResponse(std::stringstream &message);
};

#endif