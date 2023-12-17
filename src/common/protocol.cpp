/**
 * @file protocol.cpp
 * @brief Implementation of the protocols enconders and decoder to be used by
 * the Client or the Server.
 */
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

int ProtocolCommunication::readNumber(MessageSource &message, size_t size) {
    std::string string = readString(message, size);

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

std::string ProtocolCommunication::readFileName(MessageSource &message) {
    std::string filename = readString(message, PROTOCOL_FNAME_SIZE);

    if (!isValidFileName(filename)) {
        throw ProtocolViolationException();
    }

    return filename;
}

std::string ProtocolCommunication::readAuctionName(MessageSource &message) {
    std::string name = readString(message, PROTOCOL_AUCTIONNAME_SIZE);

    if (!isValidAuctionName(name)) {
        throw ProtocolViolationException();
    }

    return name;
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

void ProtocolCommunication::writeFileName(std::stringstream &message,
                                          std::string fileName) {
    if (!isValidFileName(fileName) || fileName.length() > PROTOCOL_FNAME_SIZE) {
        throw ProtocolViolationException();
    }

    writeString(message, fileName);
}

void ProtocolCommunication::writeAuctionName(std::stringstream &message,
                                             std::string name) {
    if (!isValidAuctionName(name) ||
        name.length() > PROTOCOL_AUCTIONNAME_SIZE) {
        throw ProtocolViolationException();
    }

    writeString(message, name);
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

    writeString(message, "UNR");  // Write the identifier "UNR"
    writeSpace(message);

    writeUid(message, _uid);

    writeSpace(message);

    writePassword(message, _password);

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void UnregisterCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "UNR"); The identifier is already read by the
    // server

    readSpace(message);

    _uid = readUid(message);

    readSpace(message);

    _password = readPassword(message);

    readDelimiter(message);  // Read the delimiter
}

std::stringstream UnregisterCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RUR");  // Write the identifier "RUR"
    writeSpace(message);
    writeString(message, _status);
    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void UnregisterCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RUR");  // Read the identifier "RUR"
    readSpace(message);
    // Read the status, and check if it is one of the options
    _status = readString(message, {"OK", "NOK", "UNR"});
    readDelimiter(message);  // Read the delimiter
}

std::stringstream ListUserAuctionsCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "LMA");  // Write the identifier "LMA"
    writeSpace(message);

    writeUid(message, _uid);

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void ListUserAuctionsCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "LMA"); The identifier is already read by the
    // server

    readSpace(message);

    _uid = readUid(message);

    readDelimiter(message);  // Read the delimiter
}

std::stringstream ListUserAuctionsCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RMA");  // Write the identifier "RMA"

    writeSpace(message);
    writeString(message, _status);

    for (auto auction : _auctions) {  // Write each auction
        if (auction.second != "0" && auction.second != "1") {
            throw ProtocolViolationException();
        }
        writeSpace(message);
        writeAid(message, auction.first);  // Write the AID
        writeSpace(message);
        writeString(message, auction.second);  // Write the auction state
    }

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void ListUserAuctionsCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RMA");  // Read the identifier "RMA"

    readSpace(message);

    // Read the status, checking if it is one of the options
    _status = readString(message, {"OK", "NOK", "NLG"});

    if (_status !=
        "OK") {  // If the status is not OK, read the delimiter and return
        readDelimiter(message);
        return;
    }

    while (1) {
        // Read a char, checking if it is a space or a delimiter
        char c = readChar(message, {' ', PROTOCOL_MESSAGE_DELIMITER});

        if (c == PROTOCOL_MESSAGE_DELIMITER) {
            // If it is a delimiter, return
            break;
        } else {
            std::string AID = readAid(message);

            readSpace(message);

            // Read the auction state, checking if it is one of the options
            std::string status =
                readString(message, std::vector<std::string>{"0", "1"});

            _auctions.insert({AID, status});  // Insert the auction in the map
        }
    }
}

std::stringstream ListUserBidsCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "LMB");  // Write the identifier "LMB"
    writeSpace(message);

    writeUid(message, _uid);

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void ListUserBidsCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "LMB"); The identifier is already read by the
    // server

    readSpace(message);

    _uid = readUid(message);

    readDelimiter(message);  // Read the delimiter
}

std::stringstream ListUserBidsCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RMB");  // Write the identifier "RMB"

    writeSpace(message);
    writeString(message, _status);

    for (auto auction : _bids) {  // Write each bid
        if (auction.second != "0" && auction.second != "1") {
            throw ProtocolViolationException();
        }

        writeSpace(message);
        writeAid(message, auction.first);  // Write the AID
        writeSpace(message);
        writeString(message, auction.second);  // Write the auction state
    }

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void ListUserBidsCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RMB");  // Read the identifier "RMB"

    readSpace(message);

    // Read the status, checking if it is one of the options
    _status = readString(message, {"OK", "NOK", "NLG"});

    if (_status != "OK") {
        // If the status is not OK, read the delimiter and return
        readDelimiter(message);
        return;
    }

    while (1) {
        // Read a char, checking if it is a space or a delimiter
        char c = readChar(message, {' ', PROTOCOL_MESSAGE_DELIMITER});

        if (c == PROTOCOL_MESSAGE_DELIMITER) {
            // If it is a delimiter, return
            break;
        } else {
            std::string AID = readAid(message);

            readSpace(message);

            // Read the auction state, checking if it is one of the options
            std::string status =
                readString(message, std::vector<std::string>{"0", "1"});

            _bids.insert({AID, status});  // Insert the bid in the map
        }
    }
}

std::stringstream ListAllAuctionsCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "LST");  // Write the identifier "LST"

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void ListAllAuctionsCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "LST"); The identifier is already read by the
    // server

    readDelimiter(message);  // Read the delimiter
}

std::stringstream ListAllAuctionsCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RLS");  // Write the identifier "RLS"

    writeSpace(message);
    writeString(message, _status);

    for (auto auction : _auctions) {  // Write each auction
        if (auction.second != "0" && auction.second != "1") {
            throw ProtocolViolationException();
        }
        writeSpace(message);
        writeAid(message, auction.first);  // Write the AID
        writeSpace(message);
        writeString(message, auction.second);  // Write the auction state
    }

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void ListAllAuctionsCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RLS");  // Read the identifier "RLS"

    readSpace(message);

    // Read the status, checking if it is one of the options
    _status = readString(message, std::vector<std::string>({"OK", "NOK"}));

    if (_status != "OK") {
        // If the status is not OK, read the delimiter and return
        readDelimiter(message);
        return;
    }

    while (1) {
        // Read a char, checking if it is a space or a delimiter
        char c = readChar(message, {' ', PROTOCOL_MESSAGE_DELIMITER});

        if (c == PROTOCOL_MESSAGE_DELIMITER) {
            // If it is a delimiter, return
            break;
        } else {
            std::string AID = readAid(message);

            readSpace(message);

            // Read the auction state, checking if it is one of the options
            std::string status =
                readString(message, std::vector<std::string>{"0", "1"});

            _auctions.insert({AID, status});  // Insert the auction in the map
        }
    }
}

std::stringstream ShowRecordCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "SRC");  // Write the identifier "SRC"
    writeSpace(message);

    writeAid(message, _aid);

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void ShowRecordCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "SRC"); The identifier is already read by the
    // server

    readSpace(message);

    _aid = readAid(message);

    readDelimiter(message);  // Read the delimiter
}

std::stringstream ShowRecordCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RRC");  // Write the identifier "RRC"

    writeSpace(message);

    writeString(message, _status);

    if (_status != "OK") {
        // If the status is not OK, read the delimiter and return
        writeDelimiter(message);
        return message;
    }

    writeSpace(message);

    writeUid(message, _hostUid);

    writeSpace(message);

    writeAuctionName(message, _auctionName);  // Write the auction name

    writeSpace(message);

    if (_assetFname.size() > 24) {
        // If the asset file name is too big, throw an exception
        throw ProtocolViolationException();
    }

    writeString(message, _assetFname);  // Write the asset file name

    writeSpace(message);

    writeNumber(message, _startValue);  // Write the start value

    writeSpace(message);

    writeDateTime(message, _startDateTime);  // Write the start date and time

    writeSpace(message);

    writeNumber(message, _timeActive);  // Write the time active

    for (long unsigned int i = 0; i < _bidderUids.size(); i++) {
        // Write each bid
        writeSpace(message);

        writeChar(message, 'B');

        writeSpace(message);

        writeUid(message, _bidderUids[i]);  // Write the bidder UID

        writeSpace(message);

        writeNumber(message, _bidValues[i]);  // Write the bid value

        writeSpace(message);

        writeDateTime(message, _bidDateTime[i]);  // Write the bid date and time

        writeSpace(message);

        writeNumber(message, _bidSecTimes[i]);  // Write the bid sec time
    }

    if (_hasEnded) {
        // If the auction has ended
        writeSpace(message);

        writeChar(message, 'E');

        writeSpace(message);

        writeDateTime(message, _endDateTime);  // Write the end date and time

        writeSpace(message);

        writeNumber(message, _endSecTime);  // Write the end sec time
    }

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void ShowRecordCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RRC");  // Read the identifier "RRC"

    readSpace(message);

    // Read the status, checking if it is one of the options
    _status = readString(message, std::vector<std::string>({"OK", "NOK"}));

    if (_status != "OK") {
        // If the status is not OK, read the delimiter and return
        readDelimiter(message);
        return;
    }

    readSpace(message);

    _hostUid = readUid(message);

    readSpace(message);

    _auctionName = readAuctionName(message);  // Read the auction name

    readSpace(message);

    _assetFname = readString(message, 24);  // Read the asset file name

    if (_assetFname.size() > 24) {
        // If the asset file name is too big, throw an exception
        throw ProtocolViolationException();
    }

    readSpace(message);

    _startValue =
        readNumber(message, PROTOCOL_STARTVALUE_SIZE);  // Read the start value

    readSpace(message);

    _startDateTime = readDateTime(message);  // Read the start date and time

    readSpace(message);

    _timeActive =
        readNumber(message, PROTOCOL_AUCTIONTIME_SIZE);  // Read the time active

    char c;

    while (1) {
        // Read a char, checking if it is a space or a delimiter
        c = readChar(message, {' ', PROTOCOL_MESSAGE_DELIMITER});

        if (c == PROTOCOL_MESSAGE_DELIMITER) {
            // If it is a delimiter, return
            return;
        }

        // Read a char, checking if it is 'B' or 'E
        c = readChar(message, {'B', 'E'});

        readSpace(message);

        if (c == 'B') {
            // If it is 'B', read the bid
            _bidderUids.push_back(readUid(message));  // Read the bidder UID

            readSpace(message);

            _bidValues.push_back(readNumber(
                message, PROTOCOL_STARTVALUE_SIZE));  // Read the bid value

            readSpace(message);

            _bidDateTime.push_back(
                readDateTime(message));  // Read the bid date and time

            readSpace(message);

            _bidSecTimes.push_back(readNumber(
                message, PROTOCOL_AUCTIONTIME_SIZE));  // Read the bid sec time
        }

        if (c == 'E') {
            // If it is 'E', read the end
            break;
        }
    }

    _hasEnded = true;  // The auction has ended

    _endDateTime = readDateTime(message);  // Read the end date and time

    readSpace(message);

    _endSecTime = readNumber(
        message, PROTOCOL_AUCTIONTIME_SIZE);  // Read the end sec time

    readDelimiter(message);
}

std::stringstream OpenAuctionCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "OPA");  // Write the identifier "OPA"

    writeSpace(message);

    writeUid(message, _uid);  // Write the UID

    writeSpace(message);

    writePassword(message, _password);  // Write the password

    writeSpace(message);

    writeAuctionName(message, _name);  // Write the auction name

    writeSpace(message);

    writeNumber(message, _startValue);  // Write the start value

    writeSpace(message);

    writeNumber(message, _timeActive);  // Write the time active

    writeSpace(message);

    if (_fileName.length() > 24) {
        // If the asset file name is too big, throw an exception
        throw ProtocolViolationException();
    }

    writeFileName(message, _fileName);  // Write the asset file name

    writeSpace(message);

    if (_fileSize > PROTOCOL_MAX_FILE_SIZE) {
        throw ProtocolViolationException();
    }

    writeNumber(message, _fileSize);  // Write the asset file size

    writeSpace(message);

    for (int i = 0; i < _fileSize; i++) {
        // Write each char of the asset file
        char c = readChar(_fileData);

        writeChar(message, c);
    }

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void OpenAuctionCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "OPA"); The identifier is already read by the
    // server

    readSpace(message);

    _uid = readUid(message);  // Read the UID

    readSpace(message);

    _password = readPassword(message);  // Read the password

    readSpace(message);

    _name = readAuctionName(message);  // Read the auction name

    readSpace(message);

    _startValue =
        readNumber(message, PROTOCOL_STARTVALUE_SIZE);  // Read the start value

    readSpace(message);

    _timeActive =
        readNumber(message, PROTOCOL_AUCTIONTIME_SIZE);  // Read the time active

    readSpace(message);

    _fileName = readFileName(message);  // Read the asset file name

    readSpace(message);

    _fileSize =
        readNumber(message, PROTOCOL_FSIZE_SIZE);  // Read the asset file size

    if (_fileSize > PROTOCOL_MAX_FILE_SIZE) {
        throw ProtocolViolationException();
    }

    readSpace(message);

    for (int i = 0; i < _fileSize; i++) {
        // Read each char of the asset file
        char c = readChar(message);

        writeChar(_fileData, c);
    }

    readDelimiter(message);  // Read the delimiter
}

std::stringstream OpenAuctionCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "ROA");  // Write the identifier "ROA"

    writeSpace(message);

    writeString(message, _status);  // Write the status

    if (_status == "OK") {
        // If the status is OK, write the AID
        writeSpace(message);
        writeAid(message, _aid);
    }

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void OpenAuctionCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "ROA");  // Read the identifier "ROA"

    readSpace(message);

    // Read the status, checking if it is one of the options
    _status = readString(message, {"OK", "NOK", "NLG"});

    if (_status == "OK") {
        // If the status is OK, read the AID
        readSpace(message);

        _aid = readAid(message);
    }

    readDelimiter(message);  // Read the delimiter
}

std::stringstream CloseAuctionCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "CLS");  // Write the identifier "CLS"

    writeSpace(message);

    writeUid(message, _uid);

    writeSpace(message);

    writePassword(message, _password);

    writeSpace(message);

    writeAid(message, _aid);

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void CloseAuctionCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "CLS"); The identifier is already read by the
    // server

    readSpace(message);

    _uid = readUid(message);

    readSpace(message);

    _password = readPassword(message);

    readSpace(message);

    _aid = readAid(message);

    readDelimiter(message);  // Read the delimiter
}

std::stringstream CloseAuctionCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RCL");  // Write the identifier "RCL"

    writeSpace(message);

    writeString(message, _status);

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void CloseAuctionCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RCL");  // Read the identifier "RCL"

    readSpace(message);

    // Read the status, checking if it is one of the options
    _status = readString(message, {"OK", "NLG", "EAU", "EOW", "END"});

    readDelimiter(message);
}

std::stringstream ShowAssetCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "SAS");  // Write the identifier "SAS"

    writeSpace(message);

    writeAid(message, _aid);

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void ShowAssetCommunication::decodeRequest(MessageSource &message) {
    // readIdentifier(message, "SAS"); The identifier is already read by the
    // server

    readSpace(message);

    _aid = readAid(message);

    readDelimiter(message);  // Read the delimiter
}

std::stringstream ShowAssetCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RSA");  // Write the identifier "RSA"

    writeSpace(message);

    writeString(message, _status);  // Write the status

    if (_status != "OK") {
        // If the status is not OK, write the delimiter and return
        writeDelimiter(message);
        return message;
    }

    writeSpace(message);

    if (_fileName.length() > 24) {
        // If the asset file name is too big, throw an exception
        throw ProtocolViolationException();
    }

    writeFileName(message, _fileName);  // Write the asset file name

    writeSpace(message);

    if (_fileSize > PROTOCOL_MAX_FILE_SIZE) {
        throw ProtocolViolationException();
    }

    writeNumber(message, _fileSize);  // Write the asset file size

    writeSpace(message);

    for (int i = 0; i < _fileSize; i++) {
        // Write each char of the asset file
        char c = readChar(_fileData);

        writeChar(message, c);
    }

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void ShowAssetCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RSA");  // Read the identifier "RSA"

    readSpace(message);

    // Read the status, checking if it is one of the options
    _status = readString(message, std::vector<std::string>({"OK", "NOK"}));

    if (_status != "OK") {
        // If the status is not OK, write the delimiter and return
        readDelimiter(message);

        return;
    }

    readSpace(message);

    _fileName = readFileName(message);  // Read the asset file name

    readSpace(message);

    _fileSize =
        readNumber(message, PROTOCOL_FSIZE_SIZE);  // Read the asset file size

    if (_fileSize > PROTOCOL_MAX_FILE_SIZE) {
        throw ProtocolViolationException();
    }

    readSpace(message);

    for (int i = 0; i < _fileSize; i++) {
        // Read each char of the asset file
        char c = readChar(message);

        writeChar(_fileData, c);
    }

    readDelimiter(message);  // Read the delimiter
}

std::stringstream BidCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "BID");  // Write the identifier "BID"

    writeSpace(message);

    writeUid(message, _uid);

    writeSpace(message);

    writePassword(message, _password);

    writeSpace(message);

    writeAid(message, _aid);

    writeSpace(message);

    writeNumber(message, _value);  // Write the bid value

    writeDelimiter(message);  // Put delimiter at the end

    return message;
}

void BidCommunication::decodeRequest(MessageSource &message) {
    // readString(message, "BID"); The identifier is already read by the server

    readSpace(message);

    _uid = readUid(message);

    readSpace(message);

    _password = readPassword(message);

    readSpace(message);

    _aid = readAid(message);

    readSpace(message);

    _value =
        readNumber(message, PROTOCOL_STARTVALUE_SIZE);  // Read the bid value

    readDelimiter(message);  // Read the delimiter
}

std::stringstream BidCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RBD");  // Write the identifier "RBD"

    writeSpace(message);

    writeString(message, _status);  // Write the status

    writeDelimiter(message);

    return message;
}

void BidCommunication::decodeResponse(MessageSource &message) {
    readIdentifier(message, "RBD");  // Read the identifier "RBD"

    readSpace(message);

    // Read the status, checking if it is one of the options
    _status = readString(message, {"NLG", "NOK", "ACC", "ILG", "REF"});

    readDelimiter(message);  // Read the delimiter
}
