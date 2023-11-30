#ifndef __PROTOCOL_HPP__
#define __PROTOCOL_HPP__

#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "config.hpp"
#include "utils.hpp"

/**
 * @brief Exception class for protocol-related errors.
 * 
 * This exception is thrown when there is an error while communicating with the server.
 */
class ProtocolException : public std::runtime_error {
  public:
    ProtocolException()
        : std::runtime_error(
              "There was an error while communicating with the server."){};
};

/**
 * @brief Exception thrown when a protocol violation occurs.
 * 
 * This exception is thrown when a violation of the protocol occurs during communication.
 * It is derived from the base class ProtocolException.
 */
class ProtocolViolationException : public ProtocolException {};

/**
 * @brief Exception class for protocol message errors.
 * 
 * This exception is thrown when there is an error related to protocol messages.
 * It is derived from the ProtocolException class.
 */
class ProtocolMessageErrorException : public ProtocolException {};

/**
 * @brief The ProtocolCommunication class is an abstract base class that defines the interface for communication protocols.
 * 
 * This class provides methods for encoding and decoding message content, as well as general purpose methods for parsing and encoding.
 * Subclasses should implement their own members and override the virtual methods to define the specific behavior of the protocol.
 */
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
    std::time_t readDateTime(std::stringstream &message);
    std::string readUid(std::stringstream &message);
    std::string readPassword(std::stringstream &message);
    std::string readAid(std::stringstream &message);
    void readIdentifier(std::stringstream &message, std::string identifier);

    void writeChar(std::stringstream &message, char c);
    void writeDelimiter(std::stringstream &message);
    void writeSpace(std::stringstream &message);
    void writeString(std::stringstream &message, std::string string);
    void writeNumber(std::stringstream &message, int number);
    void writeDateTime(std::stringstream &message, std::time_t time);
    void writeUid(std::stringstream &message, std::string uid);
    void writePassword(std::stringstream &message, std::string password);
    void writeAid(std::stringstream &message, std::string aid);

    virtual bool isTcp() = 0;
};

/**
 * @brief Represents a communication protocol for login functionality.
 * 
 * This class extends the ProtocolCommunication class and provides
 * request and response parameters for login communication.
 */
class LoginCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid; /**< The user ID for login request. */
    std::string _password; /**< The password for login request. */

    // Response parameters:
    std::string _status; /**< The status of the login response. */

    /**
     * @brief Encodes the login request into a stringstream.
     * 
     * @return The encoded login request as a stringstream.
     */
    std::stringstream encodeRequest();

    /**
     * @brief Decodes the login request from a stringstream.
     * 
     * @param message The stringstream containing the login request.
     */
    void decodeRequest(std::stringstream &message);

    /**
     * @brief Encodes the login response into a stringstream.
     * 
     * @return The encoded login response as a stringstream.
     */
    std::stringstream encodeResponse();

    /**
     * @brief Decodes the login response from a stringstream.
     * 
     * @param message The stringstream containing the login response.
     */
    void decodeResponse(std::stringstream &message);

    /**
     * @brief Checks if the communication is using TCP.
     * 
     * @return True if the communication is using TCP, false otherwise.
     */
    bool isTcp() { return false; };
};

class LogoutCommunication : public ProtocolCommunication {
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

    bool isTcp() { return false; };
};

class UnregisterCommunication : public ProtocolCommunication {
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

    bool isTcp() { return false; };
};

class ListUserAuctionsCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;

    // Response parameters:
    std::string _status;
    std::map<std::string, std::string> _auctions;

    std::stringstream encodeRequest();
    void decodeRequest(std::stringstream &message);
    std::stringstream encodeResponse();
    void decodeResponse(std::stringstream &message);

    bool isTcp() { return false; };
};

class ListUserBidsCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;

    // Response parameters:
    std::string _status;
    std::map<std::string, std::string> _bids;

    std::stringstream encodeRequest();
    void decodeRequest(std::stringstream &message);
    std::stringstream encodeResponse();
    void decodeResponse(std::stringstream &message);

    bool isTcp() { return false; };
};

class ListAllAuctionsCommunication : public ProtocolCommunication {
  public:
    // Request parameters:

    // Response parameters:
    std::string _status;
    std::map<std::string, std::string> _auctions;

    std::stringstream encodeRequest();
    void decodeRequest(std::stringstream &message);
    std::stringstream encodeResponse();
    void decodeResponse(std::stringstream &message);

    bool isTcp() { return false; };
};

class ShowRecordCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _aid;

    // Response parameters:
    // Auction info
    std::string _status;
    std::string _hostUid;
    std::string _auctionName;
    std::string _assetFname;
    int _startValue;
    std::time_t _startDateTime;
    int _timeActive;

    // Bidders info
    std::vector<std::string> _bidderUids;
    std::vector<int> _bidValues;
    std::vector<std::time_t> _bidDateTime;
    std::vector<int> _bidSecTimes;

    // End info
    bool _hasEnded;
    std::time_t _endDateTime;
    int _endSecTime;

    std::stringstream encodeRequest();
    void decodeRequest(std::stringstream &message);
    std::stringstream encodeResponse();
    void decodeResponse(std::stringstream &message);

    bool isTcp() { return false; };
};

class OpenAuctionCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;
    std::string _password;
    std::string _name;
    int _startValue;
    int _timeActive;
    std::string _fileName;
    int _fileSize;
    std::stringstream _fileData;

    // Response parameters:
    std::string _status;
    std::string _aid;

    std::stringstream encodeRequest();
    void decodeRequest(std::stringstream &message);
    std::stringstream encodeResponse();
    void decodeResponse(std::stringstream &message);

    bool isTcp() { return true; };
};

class CloseAuctionCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;
    std::string _password;
    std::string _aid;

    // Response parameters:
    std::string _status;

    std::stringstream encodeRequest();
    void decodeRequest(std::stringstream &message);
    std::stringstream encodeResponse();
    void decodeResponse(std::stringstream &message);

    bool isTcp() { return true; };
};

class ShowAssetCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _aid;

    // Response parameters:
    std::string _status;
    std::string _fileName;
    int _fileSize;
    std::stringstream _fileData;

    std::stringstream encodeRequest();
    void decodeRequest(std::stringstream &message);
    std::stringstream encodeResponse();
    void decodeResponse(std::stringstream &message);

    bool isTcp() { return true; };
};

class BidCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;
    std::string _password;
    std::string _aid;
    int _value;

    // Response parameters:
    std::string _status;

    std::stringstream encodeRequest();
    void decodeRequest(std::stringstream &message);
    std::stringstream encodeResponse();
    void decodeResponse(std::stringstream &message);

    bool isTcp() { return true; };
};

#endif