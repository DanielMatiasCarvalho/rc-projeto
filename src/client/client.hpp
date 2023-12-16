/**
 * @file client.hpp
 * @brief Header file of the Client class and related functions.
 */
#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <sys/stat.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "network.hpp"
#include "protocol.hpp"

/**
 * @brief Represents a user with a username and password.
 */
class User {
  private:
    std::string _username;  //The username of the user
    std::string _password;  //The password of the user

  public:
    /**
     * @brief Checks if the user is logged in.
     * @return True if the user is logged in, false otherwise.
     */
    bool isLoggedIn();

    /**
     * @brief Logs in the user with the given username and password.
     * @param username The username of the user.
     * @param password The password of the user.
     */
    void logIn(std::string username, std::string password);

    /**
     * @brief Logs out the user.
     */
    void logOut();

    /**
     * @brief Gets the username of the user.
     * @return The username of the user.
     */
    std::string getUsername();

    /**
     * @brief Gets the password of the user.
     * @return The password of the user.
     */
    std::string getPassword();
};

/**
 * @brief Represents a client that interacts with a server.
 */
class Client {
  private:
    std::string _hostname = DEFAULT_HOSTNAME;  //The hostname of the server
    std::string _port = DEFAULT_PORT;          //The port of the server
    std::string _downloadPath =
        "./auction_files/";  //The path to the directory where the downloaded files are stored

  public:
    User _user;            //The user of the client
    bool _toExit = false;  //Specifies whether the client should exit

    /**
     * @brief Constructs a Client object with the given command line arguments.
     * @param argc The number of command line arguments.
     * @param argv The array of command line arguments.
     */
    Client(int argc, char **argv);

    /**
     * @brief Displays information about the client.
     */
    void ShowInfo();

    /**
     * @brief Processes a request received from the server.
     * @param comm The communication protocol used for the request.
     */
    void processRequest(ProtocolCommunication &comm);

    /**
     * @brief Writes content to a file with the given name.
     * @param fName The name of the file.
     * @param content The content to be written to the file.
     */
    void writeFile(std::string fName, std::stringstream &content);

    /**
     * @brief Reads the content of a file with the given name.
     * @param fName The name of the file.
     * @return The content of the file as a stringstream.
     */
    std::stringstream readFile(std::string fName);

    /**
     * @brief Ensures that the download directory for the files exists.
     */
    void assureDirectory();

    /**
     * @brief Gets the size of a file with the given name.
     * @param fName The name of the file.
     * @return The size of the file in bytes.
     */
    int getFileSize(std::string fName);

    /**
     * @brief  Gets the set of characters that will precede the prompt
     * on the next command, shows the user uid if the client is logged in,
     * otherwise a simple arrow.
     * @retval the prompt that should be used.
     */
    std::string getPrompt();
};

#endif