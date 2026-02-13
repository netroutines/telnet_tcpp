#include "server.h"
#include <iostream>
#include <winsock2.h>

int main(int argc, char* argv[])
{
    int port = 10000; // default port

    if (argc > 1) {
        try {
            if (port = std::stoi(argv[1]); port <= 0 || port > 65535) {
                std::cerr << "Port out of range. Using default 10000.\n";
                port = 10000;
            }
        }
        catch (...) {
            std::cerr << "Invalid port. Using default 10000.\n";
        }
    }

    WSADATA wsa_data{};
    if (const int ws_init = WSAStartup(MAKEWORD(2, 2), &wsa_data); ws_init != 0) {
        std::cerr << "WSAStartup failed: " << ws_init << "\n";
        return 1;
    }

    const SOCKET server_socket = tcpp::create_server_socket(port);
    if (server_socket == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    tcpp::run_server(server_socket, port);

    // WSACleanup(); // redundant
}
