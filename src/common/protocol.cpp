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
    readChar(message, '\n');
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

        if (c == ' ' || c == '\n') {
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

int ProtocolCommunication::readNumber(std::stringstream &message) {
    std::string string = readString(message);

    // Check if string only contains digits
    for (auto c : string) {
        if (c < '0' || c > '9') {
            throw ProtocolViolationException();
        }
    }

    return stoi(string);
}

void ProtocolCommunication::writeChar(std::stringstream &message, char c) {
    message.put(c);

    if (!message.good()) {
        throw ProtocolViolationException();
    }
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
    writeChar(message, ' ');
    writeString(message, _uid);
    writeChar(message, ' ');
    writeString(message, _password);
    writeChar(message, '\n');

    return message;
}

void LoginCommunication::decodeRequest(std::stringstream &message) {
    readString(message, "LIN");
    readSpace(message);
    _uid = readString(message, 6);
    readSpace(message);
    _password = readString(message, 8);
    readDelimiter(message);
}

std::stringstream LoginCommunication::encodeResponse() {
    std::stringstream message;

    writeString(message, "RLI");
    writeChar(message, ' ');
    writeString(message, _status);
    writeChar(message, '\n');

    return message;
}

void LoginCommunication::decodeResponse(std::stringstream &message) {
    readString(message, "RLI");
    readSpace(message);
    _status = readString(message, 3);
    readDelimiter(message);
}