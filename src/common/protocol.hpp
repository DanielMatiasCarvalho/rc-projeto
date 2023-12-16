/**
 * @file protocol.hpp
 * @brief This file contains the declaration of various classes related to
 * communication protocols.
 */
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
 * This exception is thrown when there is an error while communicating with the
 * server.
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
 * This exception is thrown when a violation of the protocol occurs during
 * communication. It is derived from the base class ProtocolException.
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
 * @brief The MessageSource class represents a source of messages.
 *
 * This class provides an interface for retrieving characters from a message
 * source. Subclasses of MessageSource must implement the get(), good(), and
 * unget() methods.
 */
class MessageSource {
  public:
    /**
     * @brief Get the next character from the message source.
     *
     * @return The next character from the message source.
     */
    virtual char get() = 0;

    /**
     * @brief Check if the message source is in a good state.
     *
     * @return true if the message source is in a good state, false otherwise.
     */
    virtual bool good() = 0;

    /**
     * @brief Unget the last character read from the message source.
     *
     * This method allows the caller to "unread" the last character read from
     * the message source. After calling unget(), the next call to get() should
     * return the same character that was previously read.
     */
    virtual void unget() = 0;
};

/**
 * @brief Represents a message source that reads from a stringstream.
 */
class StreamMessage : public MessageSource {
  private:
    std::stringstream &_stream;

  public:
    /**
     * @brief Constructs a StreamMessage object with the given stringstream.
     *
     * @param stream The stringstream to read from.
     */
    StreamMessage(std::stringstream &stream) : _stream(stream){};

    /**
     * @brief Reads the next character from the stringstream.
     *
     * @return The next character from the stringstream.
     */
    char get() { return (char)_stream.get(); };

    /**
     * @brief Checks if the stringstream is in a good state.
     *
     * @return True if the stringstream is in a good state, false otherwise.
     */
    bool good() { return _stream.good(); };

    /**
     * @brief Puts the last character extracted from the stringstream back.
     */
    void unget() { _stream.unget(); };
};

/**
 * @brief Represents a TCP message source.
 *
 * This class provides functionality to read characters from a TCP socket
 * and store them in a buffer. It also allows ungetting characters from the
 * buffer.
 */
class TcpMessage : public MessageSource {
  private:
    int _fd;  // The file descriptor of the TCP socket.
    // The buffer to store characters read from the TCP socket.
    std::deque<char> _buffer;
    char _last;  // The last character read from the buffer.

  public:
    /**
     * @brief Constructs a TcpMessage object with the given file descriptor.
     *
     * @param fd The file descriptor of the TCP socket.
     */
    TcpMessage(int fd) : _fd(fd){};

    /**
     * @brief Fills the buffer with characters read from the TCP socket.
     *
     * This function reads characters from the TCP socket and stores them in the
     * buffer. If an error occurs during reading, a ProtocolException is thrown.
     */
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

    /**
     * @brief Gets the next character from the buffer.
     *
     * If the buffer is empty, this function calls fillBuffer() to read more
     * characters from the TCP socket. The last character read is stored in the
     * _last member variable.
     *
     * @return The next character from the buffer.
     */
    char get() {
        if (_buffer.size() == 0) {
            fillBuffer();
        }

        _last = _buffer.front();
        _buffer.pop_front();
        return _last;
    };

    /**
     * @brief Checks if the TcpMessage is in a good state.
     *
     * This function always returns true, indicating that the TcpMessage is in a
     * good state.
     *
     * @return true if the TcpMessage is in a good state, false otherwise.
     */
    bool good() { return true; };

    /**
     * @brief Puts the last character back into the buffer.
     *
     * This function adds the last character read back to the front of the
     * buffer. The character can then be read again by calling get().
     */
    void unget() { _buffer.push_front(_last); };
};

/**
 * @brief The ProtocolCommunication class is an abstract base class that defines
 * the interface for communication protocols.
 *
 * This class provides methods for encoding and decoding message content, as
 * well as general purpose methods for parsing and encoding. Subclasses should
 * implement their own members and override the virtual methods to define the
 * specific behavior of the protocol.
 */
class ProtocolCommunication {
  public:
    // Each subclass should implement their information as members.

    // Methods that encode/decode the message content into/from a string stream.

    /**
     * @brief Encodes the request into a stringstream.
     *
     * @return The encoded request as a stringstream.
     */
    virtual std::stringstream encodeRequest() = 0;

    /**
     * @brief Decodes the request from a stringstream.
     *
     * @param message The stringstream containing the request.
     */
    virtual void decodeRequest(MessageSource &message) = 0;

    /**
     * @brief Encodes the response into a stringstream.
     *
     * @return The encoded response as a stringstream.
     */
    virtual std::stringstream encodeResponse() = 0;

    /**
     * @brief Decodes the response from a stringstream.
     *
     * @param message The stringstream containing the response.
     */
    virtual void decodeResponse(MessageSource &message) = 0;

    // General purpose methods to allow parsing and encoding.

    /**
     * @brief Reads a character from a stringstream.
     *
     * @param message The stringstream to read from.
     * @return The character read from the stringstream.
     */
    char readChar(std::stringstream &message);

    /**
     * @brief Reads a character from a MessageSource.
     *
     * @param message The MessageSource to read from.
     * @return The character read from the MessageSource.
     */
    char readChar(MessageSource &message);

    /**
     * @brief Reads a character from a MessageSource.
     *
     * @param message The MessageSource to read from.
     * @param expected The expected character.
     */
    void readChar(MessageSource &message, char expected);

    /**
     * @brief Reads a character from a MessageSource.
     *
     * @param message The MessageSource to read from.
     * @param options The list of possible characters.
     * @return The character read from the MessageSource.
     */
    char readChar(MessageSource &message, std::vector<char> options);

    /**
     * @brief Reads a delimiter from a MessageSource.
     *
     * @param message The MessageSource to read from.
     */
    void readDelimiter(MessageSource &message);

    /**
     * @brief Reads a space from a MessageSource.
     *
     * @param message The MessageSource to read from.
     */
    void readSpace(MessageSource &message);

    /**
     * @brief Reads a string from a MessageSource.
     *
     * @param message The MessageSource to read from.
     * @return The string read from the MessageSource.
     */
    std::string readString(MessageSource &message);

    /**
     * @brief Reads a string from a MessageSource.
     *
     * @param message The MessageSource to read from.
     * @param n The number of characters to read.
     * @return The string read from the MessageSource.
     */
    std::string readString(MessageSource &message, size_t n);

    /**
     * @brief Reads a string from a MessageSource.
     *
     * @param message The MessageSource to read from.
     * @param expected The expected string.
     */
    void readString(MessageSource &message, std::string expected);

    /**
     * @brief Reads a string from a MessageSource.
     *
     * @param message The MessageSource to read from.
     * @param options The list of possible strings.
     * @return The string read from the MessageSource.
     */
    std::string readString(MessageSource &message,
                           std::vector<std::string> options);

    /**
     * @brief Reads a number from a MessageSource.
     *
     * @param message The MessageSource to read from.
     * @return The number read from the MessageSource.
     */
    int readNumber(MessageSource &message);

    /**
     * @brief Reads a DateTime from a MessageSource.
     *
     * @param message The MessageSource to read from.
     * @return The time_t that represents the DateTime.
     */
    std::time_t readDateTime(MessageSource &message);

    /**
     * @brief Reads a UID from a MessageSource.
     *
     * @param message The MessageSource to read from.
     * @return The UID read from the MessageSource.
     */
    std::string readUid(MessageSource &message);

    /**
     * @brief Reads a password from a MessageSource.
     *
     * @param message The MessageSource to read from.
     * @return The password read from the MessageSource.
     */
    std::string readPassword(MessageSource &message);

    /**
     * @brief Reads an AID from a MessageSource.
     *
     * @param message The MessageSource to read from.
     * @return The AID read from the MessageSource.
     */
    std::string readAid(MessageSource &message);

    std::string readFileName(MessageSource &message);

    /**
     * @brief Reads a identifier from a MessageSource.
     *
     * @param message The MessageSource to read from.
     * @param identifier The identifier to read.
     */
    void readIdentifier(MessageSource &message, std::string identifier);

    /**
     * @brief Writes a character to stringstream.
     *
     * @param message The stringstream to write to.
     * @param c The character to write.
     */
    void writeChar(std::stringstream &message, char c);

    /**
     * @brief Writes a delimiter to stringstream.
     *
     * @param message The stringstream to write to.
     */
    void writeDelimiter(std::stringstream &message);

    /**
     * @brief Writes a space to stringstream.
     *
     * @param message The stringstream to write to.
     */
    void writeSpace(std::stringstream &message);

    /**
     * @brief Writes a string to stringstream.
     *
     * @param message The stringstream to write to.
     * @param string The string to write.
     */
    void writeString(std::stringstream &message, std::string string);

    /**
     * @brief Writes a number to stringstream.
     *
     * @param message The stringstream to write to.
     * @param number The number to write.
     */
    void writeNumber(std::stringstream &message, int number);

    /**
     * @brief Writes a DateTime to stringstream.
     *
     * @param message The stringstream to write to.
     * @param time The time to write.
     */
    void writeDateTime(std::stringstream &message, std::time_t time);

    /**
     * @brief Writes a UID to stringstream.
     *
     * @param message The stringstream to write to.
     * @param uid The UID to write.
     */
    void writeUid(std::stringstream &message, std::string uid);

    /**
     * @brief Writes a password to stringstream.
     *
     * @param message The stringstream to write to.
     * @param password The password to write.
     */
    void writePassword(std::stringstream &message, std::string password);

    /**
     * @brief Writes an AID to stringstream.
     *
     * @param message The stringstream to write to.
     * @param aid The AID to write.
     */
    void writeAid(std::stringstream &message, std::string aid);

    void writeFileName(std::stringstream &message, std::string fileName);

    /**
     * @brief Checks if the communication protocol uses TCP.
     *
     * @return True if the protocol uses TCP, false otherwise.
     */
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
    std::string _uid;       // The user ID for login request.
    std::string _password;  // The password for login request.

    // Response parameters:
    std::string _status;  // The status of the login response.

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

/**
 * @brief Represents a communication protocol for logout functionality.
 *
 * This class extends the ProtocolCommunication class and provides
 * request and response parameters for logout communication.
 */
class LogoutCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;       // The user ID for logout request.
    std::string _password;  // The password for logout request.

    // Response parameters:
    std::string _status;  // The status of the logout response.

    /**
     * @brief Encodes a logout request into a stringstream.
     *
     * @return The encoded logout request as a stringstream.
     */
    std::stringstream encodeRequest();

    /**
     * @brief Decodes a logout request from a stringstream.
     *
     * @param message The stringstream containing the logout request.
     */
    void decodeRequest(MessageSource &message);

    /**
     * @brief Encodes a logout response into a stringstream.
     *
     * @return The encoded logout response as a stringstream.
     */
    std::stringstream encodeResponse();

    /**
     * @brief Decodes a logout response from a stringstream.
     *
     * @param message The stringstream containing the logout response.
     */
    void decodeResponse(MessageSource &message);

    /**
     * @brief Checks if the communication protocol uses TCP.
     *
     * @return True if the protocol uses TCP, false otherwise.
     */
    bool isTcp() { return false; };
};

/**
 * @brief Represents a communication protocol for unregister functionality.
 *
 * This class extends the ProtocolCommunication class and provides
 * request and response parameters for unregister communication.
 */
class UnregisterCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;       // The user ID for unregister request.
    std::string _password;  // The password for unregister request.

    // Response parameters:
    std::string _status;  // The status of the unregister response.

    /**
     * @brief Encodes an unregister request into a stringstream.
     *
     * @return The encoded unregister request as a stringstream.
     */
    std::stringstream encodeRequest();

    /**
     * @brief Decodes an unregister request from a stringstream.
     *
     * @param message The stringstream containing the unregister request.
     */
    void decodeRequest(MessageSource &message);

    /**
     * @brief Encodes an unregister response into a stringstream.
     *
     * @return The encoded unregister response as a stringstream.
     */
    std::stringstream encodeResponse();

    /**
     * @brief Decodes an unregister response from a stringstream.
     *
     * @param message The stringstream containing the unregister response.
     */
    void decodeResponse(MessageSource &message);

    /**
     * @brief Checks if the communication protocol uses TCP.
     *
     * @return True if the protocol uses TCP, false otherwise.
     */
    bool isTcp() { return false; };
};

/**
 * @brief Represents a communication protocol for list user auctions
 * functionality.
 *
 * This class extends the ProtocolCommunication class and provides
 * request and response parameters for list user auctions communication.
 */
class ListUserAuctionsCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;  // The user ID for list user auctions request.

    // Response parameters:
    std::string _status;  // The status of the list user auctions response.
    std::map<std::string, std::string> _auctions;  // The list of auctions.

    /**
     * @brief Encodes a list user auctions request into a stringstream.
     *
     * @return The encoded list user auctions request as a stringstream.
     */
    std::stringstream encodeRequest();

    /**
     * @brief Decodes a list user auctions request from a stringstream.
     *
     * @param message The stringstream containing the list user auctions
     * request.
     */
    void decodeRequest(MessageSource &message);

    /**
     * @brief Encodes a list user auctions response into a stringstream.
     *
     * @return The encoded list user auctions response as a stringstream.
     */
    std::stringstream encodeResponse();

    /**
     * @brief Decodes a list user auctions response from a stringstream.
     *
     * @param message The stringstream containing the list user auctions
     * response.
     */
    void decodeResponse(MessageSource &message);

    /**
     * @brief Checks if the communication protocol uses TCP.
     *
     * @return True if the protocol uses TCP, false otherwise.
     */
    bool isTcp() { return false; };
};

/**
 * @brief Represents a communication protocol for list user bids functionality.
 *
 * This class extends the ProtocolCommunication class and provides
 * request and response parameters for list user bids communication.
 */
class ListUserBidsCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;  // The user ID for list user bids request.

    // Response parameters:
    std::string _status;  // The status of the list user bids response.
    std::map<std::string, std::string> _bids;  // The list of bids.

    /**
     * @brief Encodes a list user bids request into a stringstream.
     *
     * @return The encoded list user bids request as a stringstream.
     */
    std::stringstream encodeRequest();

    /**
     * @brief Decodes a list user bids request from a stringstream.
     *
     * @param message The stringstream containing the list user bids request.
     */
    void decodeRequest(MessageSource &message);

    /**
     * @brief Encodes a list user bids response into a stringstream.
     *
     * @return The encoded list user bids response as a stringstream.
     */
    std::stringstream encodeResponse();

    /**
     * @brief Decodes a list user bids response from a stringstream.
     *
     * @param message The stringstream containing the list user bids response.
     */
    void decodeResponse(MessageSource &message);

    /**
     * @brief Checks if the communication protocol uses TCP.
     *
     * @return True if the protocol uses TCP, false otherwise.
     */
    bool isTcp() { return false; };
};

/**
 * @brief Represents a communication protocol for list all auctions
 * functionality.
 *
 * This class extends the ProtocolCommunication class and provides
 * request and response parameters for list all auctions communication.
 */
class ListAllAuctionsCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    // None

    // Response parameters:
    std::string _status;  // The status of the list all auctions response.
    std::map<std::string, std::string> _auctions;  // The list of auctions.

    /**
     * @brief Encodes a list all auctions request into a stringstream.
     *
     * @return The encoded list all auctions request as a stringstream.
     */
    std::stringstream encodeRequest();

    /**
     * @brief Decodes a list all auctions request from a stringstream.
     *
     * @param message The stringstream containing the list all auctions request.
     */
    void decodeRequest(MessageSource &message);

    /**
     * @brief Encodes a list all auctions response into a stringstream.
     *
     * @return The encoded list all auctions response as a stringstream.
     */
    std::stringstream encodeResponse();

    /**
     * @brief Decodes a list all auctions response from a stringstream.
     *
     * @param message The stringstream containing the list all auctions
     * response.
     */
    void decodeResponse(MessageSource &message);

    /**
     * @brief Checks if the communication protocol uses TCP.
     *
     * @return True if the protocol uses TCP, false otherwise.
     */
    bool isTcp() { return false; };
};

/**
 * @brief Represents a communication protocol for show record functionality.
 *
 * This class extends the ProtocolCommunication class and provides
 * request and response parameters for show record communication.
 */
class ShowRecordCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _aid;  // The auction ID for show record request.

    // Response parameters:
    // Auction info
    std::string _status;         // The status of the show record response.
    std::string _hostUid;        // The host user ID.
    std::string _auctionName;    // The auction name.
    std::string _assetFname;     // The asset file name.
    int _startValue;             // The starting value.
    std::time_t _startDateTime;  // The starting date and time.
    int _timeActive;             // The time active.

    // Bidders info
    std::vector<std::string> _bidderUids;   // The list of bidder user IDs.
    std::vector<int> _bidValues;            // The list of bid values.
    std::vector<std::time_t> _bidDateTime;  // The list of bid date and time.
    std::vector<int> _bidSecTimes;          // The list of bid seconds time.

    // End info
    bool _hasEnded = false;    // Whether the auction has ended.
    std::time_t _endDateTime;  // The end date and time.
    int _endSecTime;           // The end seconds time.

    /**
     * @brief Encodes a show record request into a stringstream.
     *
     * @return The encoded show record request as a stringstream.
     */
    std::stringstream encodeRequest();

    /**
     * @brief Decodes a show record request from a stringstream.
     *
     * @param message The stringstream containing the show record request.
     */
    void decodeRequest(MessageSource &message);

    /**
     * @brief Encodes a show record response into a stringstream.
     *
     * @return The encoded show record response as a stringstream.
     */
    std::stringstream encodeResponse();

    /**
     * @brief Decodes a show record response from a stringstream.
     *
     * @param message The stringstream containing the show record response.
     */
    void decodeResponse(MessageSource &message);

    /**
     * @brief Checks if the communication protocol uses TCP.
     *
     * @return True if the protocol uses TCP, false otherwise.
     */
    bool isTcp() { return false; };
};

/**
 * @brief Represents a communication protocol for open auction functionality.
 *
 * This class extends the ProtocolCommunication class and provides
 * request and response parameters for open auction communication.
 */
class OpenAuctionCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;       // The user ID for open auction request.
    std::string _password;  // The password for open auction request.
    std::string _name;      // The auction name for open auction request.
    int _startValue;        // The starting value for open auction request.
    int _timeActive;        // The time active for open auction request.
    std::string _fileName;  // The asset file name for open auction request.
    int _fileSize;          // The asset file size for open auction request.
    std::stringstream
        _fileData;  // The asset file data for open auction request.

    // Response parameters:
    std::string _status;  // The status of the open auction response.
    std::string _aid;     // The auction ID of the opened auction.

    /**
     * @brief Encodes an open auction request into a stringstream.
     *
     * @return The encoded open auction request as a stringstream.
     */
    std::stringstream encodeRequest();

    /**
     * @brief Decodes an open auction request from a stringstream.
     *
     * @param message The stringstream containing the open auction request.
     */
    void decodeRequest(MessageSource &message);

    /**
     * @brief Encodes an open auction response into a stringstream.
     *
     * @return The encoded open auction response as a stringstream.
     */
    std::stringstream encodeResponse();

    /**
     * @brief Decodes an open auction response from a stringstream.
     *
     * @param message The stringstream containing the open auction response.
     */
    void decodeResponse(MessageSource &message);

    /**
     * @brief Checks if the communication protocol uses TCP.
     *
     * @return True if the protocol uses TCP, false otherwise.
     */
    bool isTcp() { return true; };
};

/**
 * @brief Represents a communication protocol for close auction functionality.
 *
 * This class extends the ProtocolCommunication class and provides
 * request and response parameters for close auction communication.
 */
class CloseAuctionCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;       // The user ID for close auction request.
    std::string _password;  // The password for close auction request.
    std::string _aid;       // The auction ID for close auction request.

    // Response parameters:
    std::string _status;  // The status of the close auction response.

    /**
     * @brief Encodes a close auction request into a stringstream.
     *
     * @return The encoded close auction request as a stringstream.
     */
    std::stringstream encodeRequest();

    /**
     * @brief Decodes a close auction request from a stringstream.
     *
     * @param message The stringstream containing the close auction request.
     */
    void decodeRequest(MessageSource &message);

    /**
     * @brief Encodes a close auction response into a stringstream.
     *
     * @return The encoded close auction response as a stringstream.
     */
    std::stringstream encodeResponse();

    /**
     * @brief Decodes a close auction response from a stringstream.
     *
     * @param message The stringstream containing the close auction response.
     */
    void decodeResponse(MessageSource &message);

    /**
     * @brief Checks if the communication protocol uses TCP.
     *
     * @return True if the protocol uses TCP, false otherwise.
     */
    bool isTcp() { return true; };
};

/**
 * @brief Represents a communication protocol for show asset functionality.
 *
 * This class extends the ProtocolCommunication class and provides
 * request and response parameters for show asset communication.
 */
class ShowAssetCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _aid;  // The auction ID for show asset request.

    // Response parameters:
    std::string _status;          // The status of the show asset response.
    std::string _fileName;        // The asset file name.
    int _fileSize;                // The asset file size.
    std::stringstream _fileData;  // The asset file data.

    /**
     * @brief Encodes a show asset request into a stringstream.
     *
     * @return The encoded show asset request as a stringstream.
     */
    std::stringstream encodeRequest();

    /**
     * @brief Decodes a show asset request from a stringstream.
     *
     * @param message The stringstream containing the show asset request.
     */
    void decodeRequest(MessageSource &message);

    /**
     * @brief Encodes a show asset response into a stringstream.
     *
     * @return The encoded show asset response as a stringstream.
     */
    std::stringstream encodeResponse();

    /**
     * @brief Decodes a show asset response from a stringstream.
     *
     * @param message The stringstream containing the show asset response.
     */
    void decodeResponse(MessageSource &message);

    /**
     * @brief Checks if the communication protocol uses TCP.
     *
     * @return True if the protocol uses TCP, false otherwise.
     */
    bool isTcp() { return true; };
};

/**
 * @brief Represents a communication protocol for bid functionality.
 *
 * This class extends the ProtocolCommunication class and provides
 * request and response parameters for bid communication.
 */
class BidCommunication : public ProtocolCommunication {
  public:
    // Request parameters:
    std::string _uid;       // The user ID for bid request.
    std::string _password;  // The password for bid request.
    std::string _aid;       // The auction ID for bid request.
    int _value;             // The bid value for bid request.

    // Response parameters:
    std::string _status;  // The status of the bid response.

    /**
     * @brief Encodes a bid request into a stringstream.
     *
     * @return The encoded bid request as a stringstream.
     */
    std::stringstream encodeRequest();

    /**
     * @brief Decodes a bid request from a stringstream.
     *
     * @param message The stringstream containing the bid request.
     */
    void decodeRequest(MessageSource &message);

    /**
     * @brief Encodes a bid response into a stringstream.
     *
     * @return The encoded bid response as a stringstream.
     */
    std::stringstream encodeResponse();

    /**
     * @brief Decodes a bid response from a stringstream.
     *
     * @param message The stringstream containing the bid response.
     */
    void decodeResponse(MessageSource &message);

    /**
     * @brief Checks if the communication protocol uses TCP.
     *
     * @return True if the protocol uses TCP, false otherwise.
     */
    bool isTcp() { return true; };
};

#endif