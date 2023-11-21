#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include "protocol.hpp"
#include "network.hpp"
#include <string>

class Client {
  private:
    std::string _hostname = DEFAULT_HOSTNAME; // Perguntar se pode ser localhost
                                              // ou tem de ser 127.0.0.1
    std::string _port = DEFAULT_PORT;

  public:
    Client(int argc, char **argv);
    void ShowInfo();
};

#endif