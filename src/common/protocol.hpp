#ifndef __PROTOCOL_HPP__
#define __PROTOCOL_HPP__

#include <iostream>
#include <sstream>
#include <string>

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
    void readDelimiter(std::stringstream &message);
    void readSpace(std::stringstream &message);
    std::string readString(std::stringstream &message);
    std::string readString(std::stringstream &message, size_t n);
    void readString(std::stringstream &message, std::string expected);
    int readNumber(std::stringstream &message);
    void writeChar(std::stringstream &message, char c);
    void writeString(std::stringstream &message, std::string string);
};

class LoginCommunication : ProtocolCommunication {
  public:
    std::string uid;
    std::string password;

    std::stringstream encodeRequest();
    void decodeRequest(std::stringstream &message);
    std::stringstream encodeResponse();
    void decodeResponse(std::stringstream &message);
};

#endif