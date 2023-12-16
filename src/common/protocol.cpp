#include "protocol.hpp"

char ProtocolCommunication::readChar(std::stringstream &message) {
    char c = (char)message
                 .get();  // get() returns an int, so we need to cast it to char

    if (!message.good()) {  // if the stream is not good, throw an exception
        throw ProtocolViolationException();
    }

    return c;
}

char ProtocolCommunication::readChar(MessageSource &message) {
    char c = (char)message
                 .get();  // get() returns an int, so we need to cast it to char

    if (!message.good()) {  // if the stream is not good, throw an exception
        throw ProtocolViolationException();
    }

    return c;
}

void ProtocolCommunication::readChar(MessageSource &message, char expected) {
    if (readChar(message) != expected) {
        // if the read char is not the expected one, throw an exception
        throw ProtocolViolationException();
    }
}

char ProtocolCommunication::readChar(MessageSource &message,
                                     std::vector<char> options) {
    char c = readChar(message);  // read a char

    for (auto option : options) {
        // check if the read char is one of the options
        if (option == c) {
            return c;
        }
    }
    // if not, throw an exception
    throw ProtocolViolationException();
}

void ProtocolCommunication::readDelimiter(MessageSource &message) {
    readChar(message, PROTOCOL_MESSAGE_DELIMITER);  // read the delimiter
}

void ProtocolCommunication::readSpace(MessageSource &message) {
    readChar(message, ' ');  // read a space
}

std::string ProtocolCommunication::readString(MessageSource &message) {
    // To read an arbitrarily sized string, just read a string with a limit of
    // size equal to the maximum possible.
    return readString(message, std::string::npos);
}

std::string ProtocolCommunication::readString(MessageSource &message,
                                              size_t n) {
    std::string result;

    for (size_t i = 0; i < n; i++) {  // read n chars
        // Read a char
        char c = (char)message.get();

        if (!message.good()) {  // if the stream is not good, throw an exception
            throw ProtocolViolationException();
        }

        if (c == ' ' || c == PROTOCOL_MESSAGE_DELIMITER) {
            // If the char is a space or a delimiter, put it back in the stream
            message.unget();
            break;
        }

        // Otherwise, add it to the result
        result.push_back(c);
    }

    return result;
}

void ProtocolCommunication::readString(MessageSource &message,
                                       std::string expected) {
    if (readString(message) != expected) {
        // if the read string is not the expected one, throw an exception
        throw ProtocolViolationException();
    }
}

std::string ProtocolCommunication::readString(
    MessageSource &message, std::vector<std::string> options) {
    // Read a string
    std::string string = readString(message);

    for (auto option : options) {
        // Check if the read string is one of the options
        if (string == option) {
            return string;
        }
    }
    // If not, throw an exception
    throw ProtocolViolationException();
}

int ProtocolCommunication::readNumber(MessageSource &message) {
    std::string string = readString(message);

    // Check if string only contains digits
    if (!isNumeric(string)) {
        throw ProtocolViolationException();
    }

    return stoi(string);  // Convert string to int
}

std::time_t ProtocolCommunication::readDateTime(MessageSource &message) {
    std::stringstream stream;
    std::string aux;

    // Read the date and time in the format YYYY-MM-DD HH:MM:SS
    aux = readString(message, 4);  // Year
    stream << aux;

    readChar(message, '-');
    stream << '-';

    aux = readString(message, 2);  // Month
    stream << aux;

    readChar(message, '-');
    stream << '-';

    aux = readString(message, 2);  // Day
    stream << aux;

    readSpace(message);
    stream << ' ';

    aux = readString(message, 2);  // Hour
    stream << aux;

    readChar(message, ':');
    stream << ':';

    aux = readString(message, 2);  // Minute
    stream << aux;

    readChar(message, ':');
    stream << ':';

    aux = readString(message, 2);  // Second
    stream << aux;

    std::tm tm;

    // Convert the string to a tm struct
    stream >> std::get_time(&tm, "%Y-%m-%d%n%H:%M:%S");

    if (!message.good()) {
        throw ProtocolViolationException();
    }

    std::time_t time = std::mktime(&tm);  // Convert the tm struct to a time_t

    return time;
}

std::string ProtocolCommunication::readUid(MessageSource &message) {
    std::string uid = readString(message, PROTOCOL_UID_SIZE);  // Read a string

    if (!isNumeric(uid) || uid.length() != PROTOCOL_UID_SIZE) {
        // Check if the string only contains digits and has the correct size
        throw ProtocolViolationException();
    }

    return uid;
}

std::string ProtocolCommunication::readPassword(MessageSource &message) {
    std::string password =
        readString(message, PROTOCOL_PASSWORD_SIZE);  // Read a string

    if (!isAlphaNumeric(password) ||
        password.length() != PROTOCOL_PASSWORD_SIZE) {
        // Check if the string only contains digits and has the correct size
        throw ProtocolViolationException();
    }

    return password;
}

std::string ProtocolCommunication::readAid(MessageSource &message) {
    std::string aid = readString(message, PROTOCOL_AID_SIZE);  // Read a string

    if (!isAlphaNumeric(aid) || aid.length() != PROTOCOL_AID_SIZE) {
        // Check if the string only contains digits and has the correct size
        throw ProtocolViolationException();
    }

    return aid;
}

void ProtocolCommunication::readIdentifier(MessageSource &message,
                                           std::string identifier) {
    std::string identifierRecieved = readString(message, 3);  // Read a string

    if (identifierRecieved == PROTOCOL_ERROR_IDENTIFIER) {
        // If the identifier is the error identifier, throw an exception
        throw ProtocolMessageErrorException();
    } else if (identifierRecieved != identifier) {
        // If the identifier is not the expected one, throw an exception
        throw ProtocolViolationException();
    }
}

void ProtocolCommunication::writeChar(std::stringstream &message, char c) {
    message.put(c);  // put() takes a char as an argument

    if (!message.good()) {  // if the stream is not good, throw an exception
        throw ProtocolViolationException();
    }
}

void ProtocolCommunication::writeDelimiter(std::stringstream &message) {
    writeChar(message, PROTOCOL_MESSAGE_DELIMITER);  // write the delimiter
}

void ProtocolCommunication::writeSpace(std::stringstream &message) {
    writeChar(message, ' ');  // write a space
}

void ProtocolCommunication::writeString(std::stringstream &message,
                                        std::string string) {
    for (auto c : string) {  // write each char of the string
        writeChar(message, c);
    }
}

void ProtocolCommunication::writeNumber(std::stringstream &message,
                                        int number) {
    std::string value =
        std::to_string(number);  // convert the number to a string

    writeString(message, value);  // write the string
}

void ProtocolCommunication::writeDateTime(std::stringstream &message,
                                          std::time_t time) {
    std::tm tm = *(std::localtime(&time));  // convert the time_t to a tm struct

    // write the tm struct to the string format, then put it in the stringstream
    message << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
}

void ProtocolCommunication::writeUid(std::stringstream &message,
                                     std::string uid) {
    if (!isNumeric(uid) || uid.length() != PROTOCOL_UID_SIZE) {
        // check if the string only contains digits and has the correct size
        throw ProtocolViolationException();
    }

    writeString(message, uid);  // write the string
}

void ProtocolCommunication::writePassword(std::stringstream &message,
                                          std::string password) {
    if (!isAlphaNumeric(password) ||
        password.length() != PROTOCOL_PASSWORD_SIZE) {
        // check if the string only contains digits and has the correct size
        throw ProtocolViolationException();
    }

    writeString(message, password);  // write the string
}

void ProtocolCommunication::writeAid(std::stringstream &message,
                                     std::string aid) {
    if (!isNumeric(aid) || aid.length() != PROTOCOL_AID_SIZE) {
        // check if the string only contains digits and has the correct size
        throw ProtocolViolationException();
    }

    writeString(message, aid);  // write the string
}

std::stringstream LoginCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "LIN");  // Write the identifier "LIN"
    writeSpace(message);

    writeUid(message, _uid);

    writeSpace(message);

    writePassword(message, _password);

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void LoginCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "LIN"); The identifier is already read by the
    // server

    readSpace(message);

    _uid = readUid(message);

    readSpace(message);

    _password = readPassword(message);

    readDelimiter(message);
}

std::stringstream LoginCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RLI");  // Write the identifier "RLI"
    writeSpace(message);
    writeString(message, _status);
    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void LoginCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RLI");  // Read the identifier "RLI"
    readSpace(message);
    // Read the status, and check if it is one of the options
    _status = readString(message, {"OK", "NOK", "REG"});
    readDelimiter(message);  // Read the delimiter
}

std::stringstream LogoutCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "LOU");  // Write the identifier "LOU"
    writeSpace(message);

    writeUid(message, _uid);

    writeSpace(message);

    writePassword(message, _password);

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void LogoutCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "LOU"); The identifier is already read by the
    // server

    readSpace(message);

    _uid = readUid(message);

    readSpace(message);

    _password = readPassword(message);

    readDelimiter(message);  // Read the delimiter
}

std::stringstream LogoutCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RLO");  // Write the identifier "RLO"
    writeSpace(message);
    writeString(message, _status);
    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void LogoutCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RLO");  // Read the identifier "RLO"
    readSpace(message);
    // Read the status, and check if it is one of the options
    _status = readString(message, {"OK", "NOK", "REG"});
    readDelimiter(message);  // Read the delimiter
}

std::stringstream UnregisterCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "UNR");
    writeSpace(message);

    writeUid(message, _uid);

    writeSpace(message);

    writePassword(message, _password);

    writeDelimiter(message);

    return message;
}

void UnregisterCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "UNR");

    readSpace(message);

    _uid = readUid(message);

    readSpace(message);

    _password = readPassword(message);

    readDelimiter(message);
}

std::stringstream UnregisterCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RUR");
    writeSpace(message);
    writeString(message, _status);
    writeDelimiter(message);

    return message;
}

void UnregisterCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RUR");
    readSpace(message);
    _status = readString(message, {"OK", "NOK", "UNR"});
    readDelimiter(message);
}

std::stringstream ListUserAuctionsCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "LMA");
    writeSpace(message);

    writeUid(message, _uid);

    writeDelimiter(message);

    return message;
}

void ListUserAuctionsCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "LMA");

    readSpace(message);

    _uid = readUid(message);

    readDelimiter(message);
}

std::stringstream ListUserAuctionsCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RMA");

    writeSpace(message);
    writeString(message, _status);

    for (auto auction : _auctions) {
        if (auction.second != "0" && auction.second != "1") {
            throw ProtocolViolationException();
        }
        writeSpace(message);
        writeAid(message, auction.first);
        writeSpace(message);
        writeString(message, auction.second);
    }

    writeDelimiter(message);

    return message;
}

void ListUserAuctionsCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RMA");

    readSpace(message);

    _status = readString(message, {"OK", "NOK", "NLG"});

    if (_status != "OK") {
        readDelimiter(message);
        return;
    }

    while (1) {
        char c = readChar(message, {' ', PROTOCOL_MESSAGE_DELIMITER});

        if (c == PROTOCOL_MESSAGE_DELIMITER) {
            break;
        } else {
            std::string AID = readAid(message);

            readSpace(message);

            std::string status =
                readString(message, std::vector<std::string>{"0", "1"});

            _auctions.insert({AID, status});
        }
    }
}

std::stringstream ListUserBidsCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "LMB");
    writeSpace(message);

    writeUid(message, _uid);

    writeDelimiter(message);

    return message;
}

void ListUserBidsCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "LMB");

    readSpace(message);

    _uid = readUid(message);

    readDelimiter(message);
}

std::stringstream ListUserBidsCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RMB");

    writeSpace(message);
    writeString(message, _status);

    for (auto auction : _bids) {
        if (auction.second != "0" && auction.second != "1") {
            throw ProtocolViolationException();
        }

        writeSpace(message);
        writeAid(message, auction.first);
        writeSpace(message);
        writeString(message, auction.second);
    }

    writeDelimiter(message);

    return message;
}

void ListUserBidsCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RMB");

    readSpace(message);

    _status = readString(message, {"OK", "NOK", "NLG"});

    if (_status != "OK") {
        readDelimiter(message);
        return;
    }

    while (1) {
        char c = readChar(message, {' ', PROTOCOL_MESSAGE_DELIMITER});

        if (c == PROTOCOL_MESSAGE_DELIMITER) {
            break;
        } else {
            std::string AID = readAid(message);

            readSpace(message);

            std::string status =
                readString(message, std::vector<std::string>{"0", "1"});

            _bids.insert({AID, status});
        }
    }
}

std::stringstream ListAllAuctionsCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "LST");

    writeDelimiter(message);

    return message;
}

void ListAllAuctionsCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "LST");

    readDelimiter(message);
}

std::stringstream ListAllAuctionsCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RLS");

    writeSpace(message);
    writeString(message, _status);

    for (auto auction : _auctions) {
        if (auction.second != "0" && auction.second != "1") {
            throw ProtocolViolationException();
        }
        writeSpace(message);
        writeAid(message, auction.first);
        writeSpace(message);
        writeString(message, auction.second);
    }

    writeDelimiter(message);

    return message;
}

void ListAllAuctionsCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RLS");

    readSpace(message);

    _status = readString(message, std::vector<std::string>({"OK", "NOK"}));

    if (_status != "OK") {
        readDelimiter(message);
        return;
    }

    while (1) {
        char c = readChar(message, {' ', PROTOCOL_MESSAGE_DELIMITER});

        if (c == PROTOCOL_MESSAGE_DELIMITER) {
            break;
        } else {
            std::string AID = readAid(message);

            readSpace(message);

            std::string status =
                readString(message, std::vector<std::string>{"0", "1"});

            _auctions.insert({AID, status});
        }
    }
}

std::stringstream ShowRecordCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "SRC");
    writeSpace(message);

    writeAid(message, _aid);

    writeDelimiter(message);

    return message;
}

void ShowRecordCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "SRC");

    readSpace(message);

    _aid = readAid(message);

    readDelimiter(message);
}

std::stringstream ShowRecordCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RRC");

    writeSpace(message);

    writeString(message, _status);

    if (_status != "OK") {
        writeDelimiter(message);
        return message;
    }

    writeSpace(message);

    writeUid(message, _hostUid);

    writeSpace(message);

    if (_auctionName.size() > 10) {
        throw ProtocolViolationException();
    }

    writeString(message, _auctionName);

    writeSpace(message);

    if (_assetFname.size() > 24) {
        throw ProtocolViolationException();
    }

    writeString(message, _assetFname);

    writeSpace(message);

    writeNumber(message, _startValue);

    writeSpace(message);

    writeDateTime(message, _startDateTime);

    writeSpace(message);

    writeNumber(message, _timeActive);

    for (long unsigned int i = 0; i < _bidderUids.size(); i++) {
        writeSpace(message);

        writeChar(message, 'B');

        writeSpace(message);

        writeUid(message, _bidderUids[i]);

        writeSpace(message);

        writeNumber(message, _bidValues[i]);

        writeSpace(message);

        writeDateTime(message, _bidDateTime[i]);

        writeSpace(message);

        writeNumber(message, _bidSecTimes[i]);
    }

    if (_hasEnded) {
        writeSpace(message);

        writeChar(message, 'E');

        writeSpace(message);

        writeDateTime(message, _endDateTime);

        writeSpace(message);

        writeNumber(message, _endSecTime);
    }

    writeDelimiter(message);

    return message;
}

void ShowRecordCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RRC");

    readSpace(message);

    _status = readString(message, std::vector<std::string>({"OK", "NOK"}));

    if (_status != "OK") {
        readDelimiter(message);
        return;
    }

    readSpace(message);

    _hostUid = readUid(message);

    readSpace(message);

    _auctionName = readString(message, 10);

    if (_auctionName.size() > 10) {
        throw ProtocolViolationException();
    }

    readSpace(message);

    _assetFname = readString(message, 24);

    if (_assetFname.size() > 24) {
        throw ProtocolViolationException();
    }

    readSpace(message);

    _startValue = readNumber(message);

    readSpace(message);

    _startDateTime = readDateTime(message);

    readSpace(message);

    _timeActive = readNumber(message);

    char c;

    while (1) {
        c = readChar(message, {' ', PROTOCOL_MESSAGE_DELIMITER});

        if (c == PROTOCOL_MESSAGE_DELIMITER) {
            return;
        }

        c = readChar(message, {'B', 'E'});

        readSpace(message);

        if (c == 'B') {
            _bidderUids.push_back(readUid(message));

            readSpace(message);

            _bidValues.push_back(readNumber(message));

            readSpace(message);

            _bidDateTime.push_back(readDateTime(message));

            readSpace(message);

            _bidSecTimes.push_back(readNumber(message));
        }

        if (c == 'E') {
            break;
        }
    }

    _hasEnded = true;

    _endDateTime = readDateTime(message);

    readSpace(message);

    _endSecTime = readNumber(message);

    readDelimiter(message);
}

std::stringstream OpenAuctionCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "OPA");

    writeSpace(message);

    writeUid(message, _uid);

    writeSpace(message);

    writePassword(message, _password);

    writeSpace(message);

    if (_name.length() > 10) {
        throw ProtocolViolationException();
    }

    writeString(message, _name);

    writeSpace(message);

    writeNumber(message, _startValue);

    writeSpace(message);

    writeNumber(message, _timeActive);

    writeSpace(message);

    if (_fileName.length() > 24) {
        throw ProtocolViolationException();
    }

    writeString(message, _fileName);

    writeSpace(message);

    writeNumber(message, _fileSize);

    writeSpace(message);

    for (int i = 0; i < _fileSize; i++) {
        char c = readChar(_fileData);

        writeChar(message, c);
    }

    writeDelimiter(message);

    return message;
}

void OpenAuctionCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "OPA");

    readSpace(message);

    _uid = readUid(message);

    readSpace(message);

    _password = readPassword(message);

    readSpace(message);

    _name = readString(message, 10);

    readSpace(message);

    _startValue = readNumber(message);

    readSpace(message);

    _timeActive = readNumber(message);

    readSpace(message);

    _fileName = readString(message, 24);

    readSpace(message);

    _fileSize = readNumber(message);

    readSpace(message);

    for (int i = 0; i < _fileSize; i++) {
        char c = readChar(message);

        writeChar(_fileData, c);
    }

    readDelimiter(message);
}

std::stringstream OpenAuctionCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "ROA");

    writeSpace(message);

    writeString(message, _status);

    if (_status == "OK") {
        writeSpace(message);
        writeAid(message, _aid);
    }

    writeDelimiter(message);

    return message;
}

void OpenAuctionCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "ROA");

    readSpace(message);

    _status = readString(message, {"OK", "NOK", "NLG"});

    if (_status == "OK") {
        readSpace(message);

        _aid = readAid(message);
    }

    readDelimiter(message);
}

std::stringstream CloseAuctionCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "CLS");

    writeSpace(message);

    writeUid(message, _uid);

    writeSpace(message);

    writePassword(message, _password);

    writeSpace(message);

    writeAid(message, _aid);

    writeDelimiter(message);

    return message;
}

void CloseAuctionCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "CLS");

    readSpace(message);

    _uid = readUid(message);

    readSpace(message);

    _password = readPassword(message);

    readSpace(message);

    _aid = readAid(message);

    readDelimiter(message);
}

std::stringstream CloseAuctionCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RCL");

    writeSpace(message);

    writeString(message, _status);

    writeDelimiter(message);

    return message;
}

void CloseAuctionCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RCL");

    readSpace(message);

    _status = readString(message, {"OK", "NLG", "EAU", "EOW", "END"});

    readDelimiter(message);
}

std::stringstream ShowAssetCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "SAS");

    writeSpace(message);

    writeAid(message, _aid);

    writeDelimiter(message);

    return message;
}

void ShowAssetCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "SAS");

    readSpace(message);

    _aid = readAid(message);

    readDelimiter(message);
}

std::stringstream ShowAssetCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RSA");

    writeSpace(message);

    writeString(message, _status);

    if (_status != "OK") {
        writeDelimiter(message);
        return message;
    }

    writeSpace(message);

    if (_fileName.length() > 24) {
        throw ProtocolViolationException();
    }

    writeString(message, _fileName);

    writeSpace(message);

    writeNumber(message, _fileSize);

    writeSpace(message);

    for (int i = 0; i < _fileSize; i++) {
        char c = readChar(_fileData);

        writeChar(message, c);
    }

    writeDelimiter(message);

    return message;
}

void ShowAssetCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RSA");

    readSpace(message);

    _status = readString(message, std::vector<std::string>({"OK", "NOK"}));

    if (_status != "OK") {
        readDelimiter(message);

        return;
    }

    readSpace(message);

    _fileName = readString(message, 24);

    readSpace(message);

    _fileSize = readNumber(message);

    readSpace(message);

    for (int i = 0; i < _fileSize; i++) {
        char c = readChar(message);

        writeChar(_fileData, c);
    }

    readDelimiter(message);
}

std::stringstream BidCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "BID");

    writeSpace(message);

    writeUid(message, _uid);

    writeSpace(message);

    writePassword(message, _password);

    writeSpace(message);

    writeAid(message, _aid);

    writeSpace(message);

    writeNumber(message, _value);

    writeDelimiter(message);

    return message;
}

void BidCommunication::decodeRequest(MessageSource &message) {
    // readString(message, "BID");

    readSpace(message);

    _uid = readUid(message);

    readSpace(message);

    _password = readPassword(message);

    readSpace(message);

    _aid = readAid(message);

    readSpace(message);

    _value = readNumber(message);

    readDelimiter(message);
}

std::stringstream BidCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RBD");

    writeSpace(message);

    writeString(message, _status);

    writeDelimiter(message);

    return message;
}

void BidCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RBD");

    readSpace(message);

    _status = readString(message, {"NLG", "NOK", "ACC", "ILG", "REF"});

    readDelimiter(message);
}
