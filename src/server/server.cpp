#include "server.hpp"

int main() {
    // TODO: Implement the server
    return 1;
}

/**
 * @brief Constructs a Server object.
 * 
 * This constructor initializes the Server object with the provided command line arguments.
 * It parses the command line arguments and sets the port and verbosity options accordingly.
 * 
 * @param argc The number of command line arguments.
 * @param argv An array of command line arguments.
 */
Server::Server(int argc, char **argv) {
    char c;
    //Parse command line arguments
    while ((c = (char)getopt(argc, argv, "p:v")) != -1) {
        switch (c) {
            case 'p':  //Sets the port
                _port = optarg;
                break;
            case 'v':  //Sets the verbosity
                _verbose = true;
                break;
            default:
                break;
        }
    }
}

/**
 * @brief Displays the information about the server.
 * 
 * This function prints the port number and verbosity level of the server.
 */
void Server::ShowInfo() {
    std::cout << "Port: " << _port << std::endl
              << "Verbose: " << _verbose << std::endl;
}

/**
 * Displays a message if the server is in verbose mode.
 *
 * @param message The message to be displayed.
 */
void Server::showMessage(std::string message) {
    if (_verbose)
        std::cout << message << std::endl;
}
