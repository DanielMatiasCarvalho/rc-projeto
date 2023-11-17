#include "protocol.hpp"

char ProtocolCommunication::readChar(std::stringstream &message) {
    char c = (char)message.get();

    if (!message.good()) {
        std::cout << "Protocol error!" << std::endl;
    }

    return c;
}

void ProtocolCommunication::readChar(std::stringstream &message,
                                     char expected) {
    if (readChar(message) != expected) {
        std::cout << "Protocol error!" << std::endl;
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
            std::cout << "Protocol error!" << std::endl;
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
        std::cout << "Protocol error!" << std::endl;
    }
}

int ProtocolCommunication::readNumber(std::stringstream &message) {
    std::string string = readString(message);

    // Check if string only contains digits
    for (auto c : string) {
        if (c < '0' || c > '9') {
            std::cout << "Protocol error!" << std::endl;
        }
    }

    return stoi(string);
}

void ProtocolCommunication::writeChar(std::stringstream &message, char c) {
    message.put(c);

    if (!message.good()) {
        std::cout << "Protocol error!" << std::endl;
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
    writeString(message, uid);
    writeChar(message, ' ');
    writeString(message, password);
    writeChar(message, '\n');

    return message;
}

void LoginCommunication::decodeRequest(std::stringstream &message) {
    readString(message, "LIN");
    readSpace(message);
    uid = readString(message, 6);
    readSpace(message);
    password = readString(message, 8);
    readDelimiter(message);
}

std::stringstream LoginCommunication::encodeResponse() {
    return std::stringstream();
}

void LoginCommunication::decodeResponse(std::stringstream &message) {
    (void)message;
}