#ifndef __PROTOCOL_HPP__
#define __PROTOCOL_HPP__

#include <unistd.h>
#include <ctime>
#include <deque>
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

class MessageSource {
  public:
    virtual char get() = 0;
    virtual bool good() = 0;
    virtual void unget() = 0;
};

class StreamMessage : public MessageSource {
  private:
    std::stringstream &_stream;

  public:
    StreamMessage(std::stringstream &stream) : _stream(stream){};

    char get() { return (char)_stream.get(); };

    bool good() { return _stream.good(); };

    void unget() { _stream.unget(); };
};

class TcpMessage : public MessageSource {
  private:
    int _fd;
    std::deque<char> _buffer;
    char _last;

  public:
    TcpMessage(int fd) : _fd(fd){};

    void fillBuffer() {
        char buf[128];

        ssize_t n = read(_fd, buf, 128);

        if (n <= 0) {
            throw ProtocolViolationException();
        }

        for (int i = 0; i < n; i++) {
            _buffer.push_back(buf[i]);
        }
    }

    char get() {
        if (_buffer.size() == 0) {
            fillBuffer();
        }

        _last = _buffer.front();
        _buffer.pop_front();
        return _last;
    };

    bool good() { return true; };

    void unget() { _buffer.push_front(_last); };
};

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
    virtual void decodeRequest(MessageSource &message) = 0;
    virtual std::stringstream encodeResponse() = 0;
    virtual void decodeResponse(MessageSource &message) = 0;

    // General purpose methods to allow parsing and encoding.
    char readChar(std::stringstream &message);
    char readChar(MessageSource &message);
    void readChar(MessageSource &message, char expected);
    char readChar(MessageSource &message, std::vector<char> options);
    void readDelimiter(MessageSource &message);
    void readSpace(MessageSource &message);
    std::string readString(MessageSource &message);
    std::string readString(MessageSource &message, size_t n);
    void readString(MessageSource &message, std::string expected);
    std::string readString(MessageSource &message,
                           std::vector<std::string> options);
    int readNumber(MessageSource &message);
    std::time_t readDateTime(MessageSource &message);
    std::string readUid(MessageSource &message);
    std::string readPassword(MessageSource &message);
    std::string readAid(MessageSource &message);
    void readIdentifier(MessageSource &message, std::string identifier);

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
    std::string _uid;      /**< The user ID for login request. */
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
    void decodeRequest(MessageSource &message);

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
    void decodeResponse(MessageSource &message);

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
    void decodeRequest(MessageSource &message);
    std::stringstream encodeResponse();
    void decodeResponse(MessageSource &message);

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
    void decodeRequest(MessageSource &message);
    std::stringstream encodeResponse();
    void decodeResponse(MessageSource &message);

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
    void decodeRequest(MessageSource &message);
    std::stringstream encodeResponse();
    void decodeResponse(MessageSource &message);

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
    void decodeRequest(MessageSource &message);
    std::stringstream encodeResponse();
    void decodeResponse(MessageSource &message);

    bool isTcp() { return false; };
};

class ListAllAuctionsCommunication : public ProtocolCommunication {
  public:
    // Request parameters:

    // Response parameters:
    std::string _status;
    std::map<std::string, std::string> _auctions;

    std::stringstream encodeRequest();
    void decodeRequest(MessageSource &message);
    std::stringstream encodeResponse();
    void decodeResponse(MessageSource &message);

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
    bool _hasEnded = false;
    std::time_t _endDateTime;
    int _endSecTime;

    std::stringstream encodeRequest();
    void decodeRequest(MessageSource &message);
    std::stringstream encodeResponse();
    void decodeResponse(MessageSource &message);

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
    void decodeRequest(MessageSource &message);
    std::stringstream encodeResponse();
    void decodeResponse(MessageSource &message);

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
    void decodeRequest(MessageSource &message);
    std::stringstream encodeResponse();
    void decodeResponse(MessageSource &message);

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
    void decodeRequest(MessageSource &message);
    std::stringstream encodeResponse();
    void decodeResponse(MessageSource &message);

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
    void decodeRequest(MessageSource &message);
    std::stringstream encodeResponse();
    void decodeResponse(MessageSource &message);

    bool isTcp() { return true; };
};

#endif