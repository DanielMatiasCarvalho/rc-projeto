/**
 * @file client.cpp
 * @brief Implementation of the Client class and related functions.
 */
#include "client.hpp"
#include "command.hpp"
#include "protocol.hpp"
#include "terminal.hpp"

#include <unistd.h>
#include <iostream>

int main(int argc, char **argv) {
    Client client(
        argc,
        argv);  // Create object Client, parsing command line arguments in the construction
    CommandManager manager;  // Create object CommandManager
    Terminal terminal;       // Create object Terminal

    // Register commands to the CommandManager
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

    while (!client._toExit) {  // While the client is not exiting
        try {
            std::string command =
                terminal.readLine(client.getPrompt().c_str());  //Read a line from the terminal
            manager.readCommand(
                command,
                client);  //Reads the command  and its arguments, sending it to the correct handler
        } catch (CommandException const &
                     e) {  // If an exception is thrown, print the error message
            std::cout << e.what() << std::endl;
        } catch (ProtocolViolationException const
                     &e) {  //If there is an error in the protocol
            std::cout << e.what() << std::endl;
        } catch (ProtocolMessageErrorException const
                     &e) {  //If the server sends an error message
            std::cout << e.what() << std::endl;
        } catch (
            TimeoutException const &
                e) {  // If the server doesn't respond in time, print the error message
            std::cout << e.what() << std::endl;
        } catch (SocketException const &e) {  //Problems with the socket
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}

bool User::isLoggedIn() {
    return (
        (_username.length() != 0) &&
        (_password.length() !=
         0));  //If the username and password are not empty, the user is logged in
}

void User::logIn(std::string username, std::string password) {
    //Set the username and password
    _username = username;
    _password = password;
}

void User::logOut() {
    //Set the username and password to empty
    _username = "";
    _password = "";
}

std::string User::getUsername() {
    //Return the username
    return _username;
}

std::string User::getPassword() {
    //Return the password
    return _password;
}

Client::Client(int argc, char **argv) {
    char c;
    //Parse the command line arguments
    while ((c = (char)getopt(argc, argv, "n:p:")) != -1) {
        switch (c) {
            case 'n':  //If the argument is -n, set the hostname
                _hostname = optarg;
                break;
            case 'p':  //If the argument is -p, set the port
                _port = optarg;
                break;
            default:
                break;
        }
    }
}

void Client::ShowInfo() {
    //Print the hostname and port
    std::cout << "Hostname: " << _hostname << std::endl
              << "Port: " << _port << std::endl;
}

void Client::processRequest(ProtocolCommunication &comm) {
    std::stringstream reqMessage = comm.encodeRequest(),
                      resMessage;  //Encode the request

    if (comm.isTcp()) {  //If the communication is TCP, use TCP
        TcpClient tcpClient(_hostname, _port);  //Create a TCP client
        tcpClient.send(reqMessage);             //Send the request
        resMessage = tcpClient.receive();       //Receive the response
    } else {  //If the communication is UDP, use UDP
        UdpClient udpClient(_hostname, _port);  //Create a UDP client
        udpClient.send(reqMessage);             //Send the request
        resMessage = udpClient.receive();       //Receive the response
    }

    StreamMessage resStreamMessage(
        resMessage);  //Create a StreamMessage from the response
    comm.decodeResponse(resStreamMessage);  //Decode the response
}

void Client::writeFile(std::string fName, std::stringstream &content) {
    assureDirectory();  //Assure that the directory exists

    std::ofstream file(_downloadPath +
                       fName);  //Create a file with the given name

    char buffer[512];

    content.read(buffer, 512);

    ssize_t n = content.gcount();

    while (
        n !=
        0) {  //While there is content to be read, loops and writes to the file
        file.write(buffer, n);

        content.read(buffer, 512);
        n = content.gcount();
    }

    file.close();  //Close the file
}

std::stringstream Client::readFile(std::string fName) {
    std::stringstream content;  //Create a stringstream to store the content
    std::ifstream file(fName);  //Open the file with the given name

    char buffer[512];

    file.read(buffer, 512);

    ssize_t n = file.gcount();

    while (
        n !=
        0) {  //While there is content to be read, loops and writes to the stringstream
        content.write(buffer, n);

        file.read(buffer, 512);
        n = file.gcount();
    }

    file.close();  //Close the file

    return content;  //Return the stringstream
}

void Client::assureDirectory() {
    if (mkdir(_downloadPath.c_str(), 0777) ==
        -1) {  //If the directory doesn't exist, create it
        if (errno != EEXIST) {
            throw std::runtime_error("Couldn't write file");
        }
    }
}

int Client::getFileSize(std::string fName) {
    std::filesystem::path p(fName);  //Create a path with the given name

    return (int)std::filesystem::file_size(p);  //Return the size of the file
}

std::string Client::getPrompt() {
    if (!_user.isLoggedIn()) {
        return "> ";
    }

    return "[" + _user.getUsername() + "] > ";
}