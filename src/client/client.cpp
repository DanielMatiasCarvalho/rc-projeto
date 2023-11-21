#include "client.hpp"
#include "command.hpp"
#include "protocol.hpp"

#include <iostream>
#include <unistd.h>

int main(int argc, char **argv) {
    Client client(argc, argv);
    CommandManager manager;

    manager.registerCommand(std::make_shared<LoginCommand>());

    while (1) {
        try {
            manager.readCommand(client);
        } catch (CommandException const &e) {
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
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

    comm.decodeResponse(resMessage);
}