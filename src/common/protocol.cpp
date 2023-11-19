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

std::string
ProtocolCommunication::readString(std::stringstream &message,
                                  std::vector<std::string> options) {
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

std::stringstream LoginCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "LIN");
    writeSpace(message);

    if (_uid.length() != 6 || !isNumeric(_uid)) {
        throw ProtocolViolationException();
    }
    writeString(message, _uid);

    writeSpace(message);

    if (_password.length() != 8) {
        throw ProtocolViolationException();
    }
    writeString(message, _password);

    writeDelimiter(message);

    return message;
}

void LoginCommunication::decodeRequest(std::stringstream &message) {
    // readString(message, "LIN");

    readSpace(message);

    _uid = readString(message, 6);

    if (!isNumeric(_uid)) {
        throw ProtocolViolationException();
    }

    readSpace(message);

    _password = readString(message, 8);

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
    readString(message, "RLI");
    readSpace(message);
    _status = readString(message, {"OK", "NOK", "REG", "ERR"});
    readDelimiter(message);
}

std::stringstream LogoutCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "LOU");
    writeSpace(message);

    if (_uid.length() != 6 || !isNumeric(_uid)) {
        throw ProtocolViolationException();
    }
    writeString(message, _uid);

    writeSpace(message);

    if (_password.length() != 8) {
        throw ProtocolViolationException();
    }
    writeString(message, _password);

    writeDelimiter(message);

    return message;
}

void LogoutCommunication::decodeRequest(std::stringstream &message) {
    // readString(message, "LOU");

    readSpace(message);

    _uid = readString(message, 6);

    if (!isNumeric(_uid)) {
        throw ProtocolViolationException();
    }

    readSpace(message);

    _password = readString(message, 8);

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
    readString(message, "RLO");
    readSpace(message);
    _status = readString(message, {"OK", "NOK", "REG", "ERR"});
    readDelimiter(message);
}

std::stringstream UnregisterCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "UNR");
    writeSpace(message);

    if (_uid.length() != 6 || !isNumeric(_uid)) {
        throw ProtocolViolationException();
    }
    writeString(message, _uid);

    writeSpace(message);

    if (_password.length() != 8) {
        throw ProtocolViolationException();
    }
    writeString(message, _password);

    writeDelimiter(message);

    return message;
}

void UnregisterCommunication::decodeRequest(std::stringstream &message) {
    // readString(message, "UNR");

    readSpace(message);

    _uid = readString(message, 6);

    if (!isNumeric(_uid)) {
        throw ProtocolViolationException();
    }

    readSpace(message);

    _password = readString(message, 8);

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
    readString(message, "RUR");
    readSpace(message);
    _status = readString(message, {"OK", "NOK", "UNR", "ERR"});
    readDelimiter(message);
}

std::stringstream ListUserAuctionsCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "LMA");
    writeSpace(message);

    if (_uid.length() != 6 || !isNumeric(_uid)) {
        throw ProtocolViolationException();
    }
    writeString(message, _uid);

    writeDelimiter(message);

    return message;
}

void ListUserAuctionsCommunication::decodeRequest(std::stringstream &message) {
    // readString(message, "LMA");

    readSpace(message);

    _uid = readString(message, 6);

    if (!isNumeric(_uid)) {
        throw ProtocolViolationException();
    }

    readDelimiter(message);
}

std::stringstream ListUserAuctionsCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RMA");

    writeSpace(message);
    writeString(message, _status);

    for (auto auction : _auctions) {
        if (!isNumeric(auction.first) ||
            (auction.second != "0" && auction.second != "1")) {
            throw ProtocolViolationException();
        }
        writeSpace(message);
        writeString(message, auction.first);
        writeSpace(message);
        writeString(message, auction.second);
    }

    writeDelimiter(message);

    return message;
}

void ListUserAuctionsCommunication::decodeResponse(std::stringstream &message) {
    readString(message, "RMA");

    readSpace(message);

    _status = readString(message, {"OK", "NOK", "NLG", "ERR"});

    if (_status != "OK") {
        readDelimiter(message);
        return;
    }

    while (1) {
        char c = readChar(message, {' ', PROTOCOL_MESSAGE_DELIMITER});

        if (c == PROTOCOL_MESSAGE_DELIMITER) {
            break;
        } else {
            std::string AID = readString(message, 3);

            if (AID.length() != 3 || !isNumeric(AID)) {
                throw ProtocolViolationException();
            }

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

    if (_uid.length() != 6 || !isNumeric(_uid)) {
        throw ProtocolViolationException();
    }
    writeString(message, _uid);

    writeDelimiter(message);

    return message;
}

void ListUserBidsCommunication::decodeRequest(std::stringstream &message) {
    // readString(message, "LMB");

    readSpace(message);

    _uid = readString(message, 6);

    if (!isNumeric(_uid)) {
        throw ProtocolViolationException();
    }

    readDelimiter(message);
}

std::stringstream ListUserBidsCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RMB");

    writeSpace(message);
    writeString(message, _status);

    for (auto auction : _bids) {
        if (!isNumeric(auction.first) ||
            (auction.second != "0" && auction.second != "1")) {
            throw ProtocolViolationException();
        }
        writeSpace(message);
        writeString(message, auction.first);
        writeSpace(message);
        writeString(message, auction.second);
    }

    writeDelimiter(message);

    return message;
}

void ListUserBidsCommunication::decodeResponse(std::stringstream &message) {
    readString(message, "RMB");

    readSpace(message);

    _status = readString(message, {"OK", "NOK", "NLG", "ERR"});

    if (_status != "OK") {
        readDelimiter(message);
        return;
    }

    while (1) {
        char c = readChar(message, {' ', PROTOCOL_MESSAGE_DELIMITER});

        if (c == PROTOCOL_MESSAGE_DELIMITER) {
            break;
        } else {
            std::string AID = readString(message, 3);

            if (AID.length() != 3 || !isNumeric(AID)) {
                throw ProtocolViolationException();
            }

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
    // readString(message, "LST");

    readDelimiter(message);
}

std::stringstream ListAllAuctionsCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RLS");

    writeSpace(message);
    writeString(message, _status);

    for (auto auction : _auctions) {
        if (!isNumeric(auction.first) ||
            (auction.second != "0" && auction.second != "1")) {
            throw ProtocolViolationException();
        }
        writeSpace(message);
        writeString(message, auction.first);
        writeSpace(message);
        writeString(message, auction.second);
    }

    writeDelimiter(message);

    return message;
}

void ListAllAuctionsCommunication::decodeResponse(std::stringstream &message) {
    readString(message, "RLS");

    readSpace(message);

    _status = readString(message, {"OK", "NOK", "ERR"});

    if (_status != "OK") {
        readDelimiter(message);
        return;
    }

    while (1) {
        char c = readChar(message, {' ', PROTOCOL_MESSAGE_DELIMITER});

        if (c == PROTOCOL_MESSAGE_DELIMITER) {
            break;
        } else {
            std::string AID = readString(message, 3);

            if (AID.length() != 3 || !isNumeric(AID)) {
                throw ProtocolViolationException();
            }

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

    if (_aid.length() != 3 || !isNumeric(_aid)) {
        throw ProtocolViolationException();
    }

    writeString(message, _aid);

    writeDelimiter(message);

    return message;
}

void ShowRecordCommunication::decodeRequest(std::stringstream &message) {
    // readString(message, "SRC");

    readSpace(message);

    _aid = readString(message, 3);

    if (!isNumeric(_aid)) {
        throw ProtocolViolationException();
    }

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

    if (!isNumeric(_hostUid) || _hostUid.size() != 6) {
        throw ProtocolViolationException();
    }

    writeString(message, _hostUid);

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

        if (!isNumeric(_bidderUids[i]) || _bidderUids[i].size() != 6) {
            throw ProtocolViolationException();
        }

        writeString(message, _bidderUids[i]);

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
    readString(message, "RRC");

    readSpace(message);

    _status = readString(message, {"OK", "NOK", "ERR"});

    if (_status != "OK") {
        readDelimiter(message);
        return;
    }

    readSpace(message);

    _hostUid = readString(message, 6);

    if (!isNumeric(_hostUid) || _hostUid.size() != 6) {
        throw ProtocolViolationException();
    }

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

    while (1) {
        char c = readChar(message, {' ', PROTOCOL_MESSAGE_DELIMITER});

        if (c == PROTOCOL_MESSAGE_DELIMITER) {
            break;
        }

        char decider = readChar(message, {'B', 'E'});

        readSpace(message);

        if (decider == 'B') {

            std::string bidderUid = readString(message, 6);

            if (!isNumeric(bidderUid) || bidderUid.size() != 6) {
                throw ProtocolViolationException();
            }

            _bidderUids.push_back(bidderUid);

            readSpace(message);

            int bidValue = readNumber(message);

            _bidValues.push_back(bidValue);

            readSpace(message);

            std::time_t bidDateTime = readDateTime(message);

            _bidDateTime.push_back(bidDateTime);

            readSpace(message);

            int bidSecTime = readNumber(message);

            _bidSecTimes.push_back(bidSecTime);
        }

        if (decider == 'E') {
            _hasEnded = true;

            _endDateTime = readDateTime(message);

            readSpace(message);

            _endSecTime = readNumber(message);
        }
    }
}

std::stringstream OpenAuctionCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "OPA");

    writeSpace(message);

    if (!isNumeric(_uid) || _uid.length() != 6) {
        throw ProtocolViolationException();
    }

    writeString(message, _uid);

    writeSpace(message);

    if (_password.length() != 8) {
        throw ProtocolViolationException();
    }

    writeString(message, _password);

    writeSpace(message);

    if (_name.length() > 10) {
        throw ProtocolViolationException();
    }

    writeString(message, _name);

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
    // readString(message, "OPA");

    readSpace(message);

    _uid = readString(message, 6);

    if (!isNumeric(_uid) || _uid.length() != 6) {
        throw ProtocolViolationException();
    }

    readSpace(message);

    _password = readString(message, 6);

    if (_password.length() != 8) {
        throw ProtocolViolationException();
    }

    readSpace(message);

    _name = readString(message, 10);

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
        if (_aid.length() != 3) {
            throw ProtocolViolationException();
        }

        writeSpace(message);

        writeString(message, _aid);
    }

    writeDelimiter(message);

    return message;
}

void OpenAuctionCommunication::decodeResponse(std::stringstream &message) {
    // readString(message, "ROA");

    readSpace(message);

    _status = readString(message, {"OK", "NOK", "NLG", "ERR"});

    if (_status == "OK") {
        readSpace(message);

        _aid = readString(message, 3);

        if (!isNumeric(_aid) || _aid.length() != 3) {
            throw ProtocolViolationException();
        }
    }

    readDelimiter(message);
}

std::stringstream CloseAuctionCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "CLS");

    writeSpace(message);

    if (!isNumeric(_uid) || _uid.length() != 6) {
        throw ProtocolViolationException();
    }

    writeString(message, _uid);

    writeSpace(message);

    if (_password.length() != 8) {
        throw ProtocolViolationException();
    }

    writeString(message, _password);

    writeSpace(message);

    if (!isNumeric(_aid) || _aid.length() != 3) {
        throw ProtocolViolationException();
    }

    writeString(message, _aid);

    writeDelimiter(message);
}

void CloseAuctionCommunication::decodeRequest(std::stringstream &message) {
    // readString(message, "CLS");

    readSpace(message);

    _aid = readString(message, 6);

    if (!isNumeric(_aid) || _aid.length() != 6) {
        throw ProtocolViolationException();
    }

    readSpace(message);

    _password = readString(message, 8);

    if (_password.length() != 8) {
        throw ProtocolViolationException();
    }

    readSpace(message);

    _aid = readString(message, 3);

    if (!isNumeric(_aid) || _aid.length() != 3) {
        throw ProtocolViolationException();
    }

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
    readString(message, "RCL");

    readSpace(message);

    readString(message, {"OK", "NLG", "EAU", "AID", "EOW", "END"});

    readDelimiter(message);
}