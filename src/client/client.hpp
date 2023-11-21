#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include "network.hpp"
#include "protocol.hpp"
#include <string>

class User {
  private:
    std::string _username;
    std::string _password;
  
  public:
    bool isLoggedIn();
    void logIn(std::string username, std::string password);
    void logOut();
    std::string getUsername();
    std::string getPassword();
};

class Client {
  private:
    std::string _hostname = DEFAULT_HOSTNAME; 
    std::string _port = DEFAULT_PORT;

  public:
    User _user;
    bool _toExit = false;
    Client(int argc, char **argv);
    void ShowInfo();
    void processRequest(ProtocolCommunication &comm);
};

#endif