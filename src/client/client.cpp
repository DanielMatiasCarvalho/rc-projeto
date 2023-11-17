#include "client.hpp"
#include "command.hpp"
#include "protocol.hpp"

#include <iostream>
#include <unistd.h>

int main(int argc, char **argv) {
    ClientState state(argc, argv);
    CommandManager manager;

    manager.registerCommand(std::make_shared<LoginCommand>());

    while (1) {
        manager.readCommand(state);
    }

    return 0;
}

ClientState::ClientState(int argc, char **argv) {
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

void ClientState::ShowInfo() {
    std::cout << "Hostname: " << _hostname << std::endl
              << "Port: " << _port << std::endl;
}