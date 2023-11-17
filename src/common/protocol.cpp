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

std::stringstream LoginCommunication::encodeRequest() {
    std::stringstream message;

    writeString(message, "LIN");
    writeSpace(message);

    if (_uid.length() != 6) {
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

    if (_uid.length() != 6) {
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

    if (_uid.length() != 6) {
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