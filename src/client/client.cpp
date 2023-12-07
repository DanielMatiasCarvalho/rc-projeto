#include "client.hpp"
#include "command.hpp"
#include "protocol.hpp"

#include <unistd.h>
#include <iostream>

int main(int argc, char **argv) {
    Client client(argc, argv);
    CommandManager manager;

    manager.registerCommand(std::make_shared<LoginCommand>());
    manager.registerCommand(std::make_shared<LogoutCommand>());
    manager.registerCommand(std::make_shared<UnregisterCommand>());
    manager.registerCommand(std::make_shared<ExitCommand>());
    manager.registerCommand(std::make_shared<OpenCommand>());
    manager.registerCommand(std::make_shared<CloseCommand>());
    manager.registerCommand(std::make_shared<ListUserAuctionsCommand>());
    manager.registerCommand(std::make_shared<ListUserBidsCommand>());
    manager.registerCommand(std::make_shared<ListAllAuctionsCommand>());
    manager.registerCommand(std::make_shared<ShowAssetCommand>());
    manager.registerCommand(std::make_shared<BidCommand>());
    manager.registerCommand(std::make_shared<ShowRecordCommand>());

    while (!client._toExit) {
        try {
            manager.readCommand(client);
        } catch (CommandException const &e) {
            std::cout << e.what() << std::endl;
        } catch (ProtocolViolationException const &e) {
            std::cout << e.what() << std::endl;
        } catch (ProtocolMessageErrorException const &e) {
            std::cout << e.what() << std::endl;
        } catch (TimeoutException const &e) {
            std::cout << e.what() << std::endl;
        } catch (SocketException const &e) {
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}

bool User::isLoggedIn() {
    return ((_username.length() != 0) && (_password.length() != 0));
}

void User::logIn(std::string username, std::string password) {
    _username = username;
    _password = password;
}

void User::logOut() {
    _username = "";
    _password = "";
}

std::string User::getUsername() {
    return _username;
}

std::string User::getPassword() {
    return _password;
}

Client::Client(int argc, char **argv) {
    char c;

    while ((c = (char)getopt(argc, argv, "n:p:")) != -1) {
        switch (c) {
            case 'n':
                _hostname = optarg;
                break;
            case 'p':
                _port = optarg;
                break;
            default:
                break;
        }
    }
}

void Client::ShowInfo() {
    std::cout << "Hostname: " << _hostname << std::endl
              << "Port: " << _port << std::endl;
}

void Client::processRequest(ProtocolCommunication &comm) {
    std::stringstream reqMessage = comm.encodeRequest(), resMessage;

    if (comm.isTcp()) {
        TcpClient tcpClient(_hostname, _port);
        tcpClient.send(reqMessage);
        resMessage = tcpClient.receive();
    } else {
        UdpClient udpClient(_hostname, _port);
        udpClient.send(reqMessage);
        resMessage = udpClient.receive();
    }

    StreamMessage resStreamMessage(resMessage);
    comm.decodeResponse(resStreamMessage);
}

void Client::writeFile(std::string fName, std::stringstream &content) {
    assureDirectory();

    std::ofstream file(_downloadPath + fName);

    char buffer[512];

    content.read(buffer, 512);

    ssize_t n = content.gcount();

    while (n != 0) {
        file.write(buffer, n);

        content.read(buffer, 512);
        n = content.gcount();
    }

    file.close();
}

std::stringstream Client::readFile(std::string fName) {
    std::stringstream content;
    std::ifstream file(fName);

    char buffer[512];

    file.read(buffer, 512);

    ssize_t n = file.gcount();

    while (n != 0) {
        content.write(buffer, n);

        file.read(buffer, 512);
        n = file.gcount();
    }

    file.close();

    return content;
}

void Client::assureDirectory() {
    if (mkdir(_downloadPath.c_str(), 0777) == -1) {
        if (errno != EEXIST) {
            throw std::runtime_error("Couldn't write file");
        }
    }
}

int Client::getFileSize(std::string fName) {
    std::filesystem::path p(fName);

    return (int)std::filesystem::file_size(p);
}