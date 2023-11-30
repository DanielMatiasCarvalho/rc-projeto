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

/**
 * @brief Checks if the user is logged in.
 * 
 * @return true if the user is logged in, false otherwise.
 */
bool User::isLoggedIn() {
    return ((_username.length() != 0) && (_password.length() != 0));
}

/**
 * Logs in the user with the given username and password.
 * 
 * @param username The username of the user.
 * @param password The password of the user.
 */
void User::logIn(std::string username, std::string password) {
    _username = username;
    _password = password;
}

/**
 * @brief Logs out the user.
 */
void User::logOut() {
    _username = "";
    _password = "";
}

/**
 * @brief Get the username of the user.
 * 
 * @return The username of the user as a string.
 */
std::string User::getUsername() {
    return _username;
}

/**
 * @brief Retrieves the password of the user.
 * 
 * @return The password of the user as a string.
 */
std::string User::getPassword() {
    return _password;
}

/**
 * @brief Constructor for the Client class.
 * 
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments.
 */
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

/**
 * @brief Displays information about the client.
 */
void Client::ShowInfo() {
    std::cout << "Hostname: " << _hostname << std::endl
              << "Port: " << _port << std::endl;
}

/**
 * Process the request using the given communication protocol.
 *
 * @param comm The communication protocol to use.
 */
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

    comm.decodeResponse(resMessage);
}

/**
 * Writes the content of a stringstream to a file.
 * 
 * @param fName The name of the file to write to.
 * @param content The stringstream containing the content to write.
 */
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

/**
 * @brief Reads the contents of a file and returns it as a stringstream.
 * 
 * @param fName The name of the file to be read.
 * @return The contents of the file as a stringstream.
 */
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

/**
 * @brief Assures the existence of the directory for the client.
 */
void Client::assureDirectory() {
    if (mkdir(_downloadPath.c_str(), 0777) == -1) {
        if (errno != EEXIST) {
            throw std::runtime_error("Couldn't write file");
        }
    }
}

/**
 * @brief Get the size of a file.
 * 
 * @param fName The name of the file.
 * @return The size of the file in bytes.
 */
int Client::getFileSize(std::string fName) {
    std::filesystem::path p(fName);

    return (int)std::filesystem::file_size(p);
}