#include "protocol.hpp"

char ProtocolCommunication::readChar(std::stringstream &message) {
    char c = (char)message.get();

    if (!message.good()) {
        throw ProtocolViolationException();
    }

    return c;
}

void ProtocolCommunication::readChar(std::stringstream &message,
                                     char expected) {
    if (readChar(message) != expected) {
        throw ProtocolViolationException();
    }
}

char ProtocolCommunication::readChar(std::stringstream &message,
                                     std::vector<char> options) {
    char c = readChar(message);

    for (auto option : options) {
        if (option == c) {
            return c;
        }
    }

    throw ProtocolViolationException();
}

void ProtocolCommunication::readDelimiter(std::stringstream &message) {
    readChar(message, PROTOCOL_MESSAGE_DELIMITER);
}

void ProtocolCommunication::readSpace(std::stringstream &message) {
    readChar(message, ' ');
}

std::string ProtocolCommunication::readString(std::stringstream &message) {
    // To read an arbitrarily sized string, just read a string with a limit of
    // size equal to the maximum possible.
    return readString(message, std::string::npos);
}

std::string ProtocolCommunication::readString(std::stringstream &message,
                                              size_t n) {
    std::string result;

    for (size_t i = 0; i < n; i++) {
        char c = (char)message.get();

        if (!message.good()) {
            throw ProtocolViolationException();
        }

        if (c == ' ' || c == PROTOCOL_MESSAGE_DELIMITER) {
            message.unget();
            break;
        }

        result.push_back(c);
    }

    return result;
}

void ProtocolCommunication::readString(std::stringstream &message,
                                       std::string expected) {
    if (readString(message) != expected) {
        throw ProtocolViolationException();
    }
}

std::string ProtocolCommunication::readString(
    std::stringstream &message, std::vector<std::string> options) {
    std::string string = readString(message);

    for (auto option : options) {
        if (string == option) {
            return string;
        }
    }

    throw ProtocolViolationException();
}

int ProtocolCommunication::readNumber(std::stringstream &message) {
    std::string string = readString(message);

    // Check if string only contains digits
    if (!isNumeric(string)) {
        throw ProtocolViolationException();
    }

    return stoi(string);
}

std::time_t ProtocolCommunication::readDateTime(std::stringstream &message) {
    std::stringstream stream;
    std::string aux;

    aux = readString(message, 4);
    stream << aux;

    readChar(message, '-');
    stream << '-';

    aux = readString(message, 2);
    stream << aux;

    readChar(message, '-');
    stream << '-';

    aux = readString(message, 2);
    stream << aux;

    readSpace(message);
    stream << ' ';

    aux = readString(message, 2);
    stream << aux;

    readChar(message, ':');
    stream << ':';

    aux = readString(message, 2);
    stream << aux;

    readChar(message, ':');
    stream << ':';

    aux = readString(message, 2);
    stream << aux;

    std::tm tm;

    stream >> std::get_time(&tm, "%Y-%m-%d%n%H:%M:%S");

    if (!message) {
        throw ProtocolViolationException();
    }

    std::time_t time = std::mktime(&tm);

    return time;
}

std::string ProtocolCommunication::readUid(std::stringstream &message) {
    std::string uid = readString(message, PROTOCOL_UID_SIZE);

    if (!isNumeric(uid) || uid.length() != PROTOCOL_UID_SIZE) {
        throw ProtocolViolationException();
    }

    return uid;
}

std::string ProtocolCommunication::readPassword(std::stringstream &message) {
    std::string password = readString(message, PROTOCOL_PASSWORD_SIZE);

    if (!isAlphaNumeric(password) ||
        password.length() != PROTOCOL_PASSWORD_SIZE) {
        throw ProtocolViolationException();
    }

    return password;
}

std::string ProtocolCommunication::readAid(std::stringstream &message) {
    std::string aid = readString(message, PROTOCOL_AID_SIZE);

    if (!isAlphaNumeric(aid) || aid.length() != PROTOCOL_AID_SIZE) {
        throw ProtocolViolationException();
    }

    return aid;
}

void ProtocolCommunication::readIdentifier(std::stringstream &message,
                                           std::string identifier) {
    std::string identifierRecieved = readString(message, 3);

    if (identifierRecieved == PROTOCOL_ERROR_IDENTIFIER) {
        throw ProtocolMessageErrorException();
    } else if (identifierRecieved != identifier) {
        throw ProtocolViolationException();
    }
}

void ProtocolCommunication::writeChar(std::stringstream &message, char c) {
    message.put(c);

    if (!message.good()) {
        throw ProtocolViolationException();
    }
}

void ProtocolCommunication::writeDelimiter(std::stringstream &message) {
    writeChar(message, PROTOCOL_MESSAGE_DELIMITER);
}

void ProtocolCommunication::writeSpace(std::stringstream &message) {
    writeChar(message, ' ');
}

void ProtocolCommunication::writeString(std::stringstream &message,
                                        std::string string) {
    for (auto c : string) {
        writeChar(message, c);
    }
}

void ProtocolCommunication::writeNumber(std::stringstream &message,
                                        int number) {
    std::string value = std::to_string(number);

    writeString(message, value);
}

void ProtocolCommunication::writeDateTime(std::stringstream &message,
                                          std::time_t time) {
    std::tm tm = *(std::localtime(&time));

    message << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
}

void ProtocolCommunication::writeUid(std::stringstream &message,
                                     std::string uid) {
    if (!isNumeric(uid) || uid.length() != PROTOCOL_UID_SIZE) {
        throw ProtocolViolationException();
    }

    writeString(message, uid);
}

void ProtocolCommunication::writePassword(std::stringstream &message,
                                          std::string password) {
    if (!isAlphaNumeric(password) ||
        password.length() != PROTOCOL_PASSWORD_SIZE) {
        throw ProtocolViolationException();
    }

    writeString(message, password);
}

void ProtocolCommunication::writeAid(std::stringstream &message,
                                     std::string aid) {
    if (!isNumeric(aid) || aid.length() != PROTOCOL_AID_SIZE) {
        throw ProtocolViolationException();
    }

    writeString(message, aid);
}

std::stringstream LoginCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "LIN");
    writeSpace(message);

    writeUid(message, _uid);

    writeSpace(message);

    writePassword(message, _password);

    writeDelimiter(message);

    return message;
}

void LoginCommunication::decodeRequest(std::stringstream &message) {
    // readIdentifier(message, "LIN");

    readSpace(message);

    _uid = readUid(message);

    readSpace(message);

    _password = readPassword(message);

    readDelimiter(message);
}

std::stringstream LoginCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RLI");
    writeSpace(message);
    writeString(message, _status);
    writeDelimiter(message);

    return message;
}

void LoginCommunication::decodeResponse(std::stringstream &message) {
    readIdentifier(message, "RLI");
    readSpace(message);
    _status = readString(message, {"OK", "NOK", "REG"});
    readDelimiter(message);
}

std::stringstream LogoutCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "LOU");
    writeSpace(message);

    writeUid(message, _uid);

    writeSpace(message);

    writePassword(message, _password);

    writeDelimiter(message);

    return message;
}

void LogoutCommunication::decodeRequest(std::stringstream &message) {
    // readIdentifier(message, "LOU");

    readSpace(message);

    _uid = readUid(message);

    readSpace(message);

    _password = readPassword(message);

    readDelimiter(message);
}

std::stringstream LogoutCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RLO");
    writeSpace(message);
    writeString(message, _status);
    writeDelimiter(message);

    return message;
}

void LogoutCommunication::decodeResponse(std::stringstream &message) {
    readIdentifier(message, "RLO");
    readSpace(message);
    _status = readString(message, {"OK", "NOK", "REG"});
    readDelimiter(message);
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

void UnregisterCommunication::decodeRequest(std::stringstream &message) {
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

void UnregisterCommunication::decodeResponse(std::stringstream &message) {
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

void ListUserAuctionsCommunication::decodeRequest(std::stringstream &message) {
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

void ListUserAuctionsCommunication::decodeResponse(std::stringstream &message) {
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

void ListUserBidsCommunication::decodeRequest(std::stringstream &message) {
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

void ListUserBidsCommunication::decodeResponse(std::stringstream &message) {
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

void ListAllAuctionsCommunication::decodeRequest(std::stringstream &message) {
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

void ListAllAuctionsCommunication::decodeResponse(std::stringstream &message) {
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

void ShowRecordCommunication::decodeRequest(std::stringstream &message) {
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

void ShowRecordCommunication::decodeResponse(std::stringstream &message) {
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

void OpenAuctionCommunication::decodeRequest(std::stringstream &message) {
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

void OpenAuctionCommunication::decodeResponse(std::stringstream &message) {
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

void CloseAuctionCommunication::decodeRequest(std::stringstream &message) {
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

void CloseAuctionCommunication::decodeResponse(std::stringstream &message) {
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

void ShowAssetCommunication::decodeRequest(std::stringstream &message) {
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

void ShowAssetCommunication::decodeResponse(std::stringstream &message) {
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

void BidCommunication::decodeRequest(std::stringstream &message) {
    readString(message, "BID");

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

void BidCommunication::decodeResponse(std::stringstream &message) {
    readIdentifier(message, "RBD");

    readSpace(message);

    _status = readString(message, {"NLG", "NOK", "ACC", "ILG", "REF"});

    readDelimiter(message);
}
