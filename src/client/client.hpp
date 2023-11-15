#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

class ClientState {
    private:
        std::string _hostname = "127.0.0.1"; // Perguntar se pode ser localhost ou tem de ser 127.0.0.1
        std::string _port = "58000";

    public:
        ClientState(int argc, char** argv);
        void ShowInfo();
};

#endif