#include "server.h"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>

namespace tcpp {
    bool send_all(SOCKET sock, const char* data, int len)
    {
        int total = 0;
        while (total < len) {
            const int sent = send(sock, data + total, len - total, 0);
            if (sent <= 0) return false;
            total += sent;
        }
        return true;
    }

    void handle_client(SOCKET client_socket)
    {
        std::string buffer_accum;
        char buffer[4096];

        while (true) {
            const int received = recv(client_socket, buffer, sizeof(buffer), 0);

            if (received <= 0) {
                std::cout << "Client disconnected.\n";
                closesocket(client_socket);
                return;
            }

            buffer_accum.append(buffer, received);

            std::erase_if(buffer_accum, [](unsigned char c) -> bool {
                return c < 32 && c != '\r' && c != '\n';
            });

            size_t pos;

            while ((pos = buffer_accum.find_first_of("\r\n")) != std::string::npos) {
                std::string message = buffer_accum.substr(0, pos);
                buffer_accum.erase(0, pos + 1);

                std::erase(message, '\r');
                std::erase(message, '\n');

                message.erase(0, message.find_first_not_of(" \t"));
                message.erase(message.find_last_not_of(" \t") + 1);

                if (message.empty()) continue;

                std::cout << "Received: " << message << "\n";

                std::string cmd = message;
                std::ranges::transform(cmd, cmd.begin(), [](unsigned char c) -> int { return std::toupper(c); });

                const auto start = std::chrono::steady_clock::now();

                std::string response;

                if (cmd == "PING") {
                    response = "PONG\r\n";
                }
                else if (cmd == "TIME") {
                    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                    response = std::string(std::ctime(&now));
                }
                else if (cmd == "EXIT") {
                    std::cout << "Client requested disconnect.\n";
                    closesocket(client_socket);
                    return;
                }
                else if (cmd.rfind("ECHO ", 0) == 0) {
                    response = message.substr(5) + "\r\n";
                }
                else {
                    response = "UNKNOWN COMMAND\r\n";
                }

                const auto end = std::chrono::steady_clock::now();
                const auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
                const auto elapsed_us = elapsed_ns / 1'000;
                const auto elapsed_ms = elapsed_ns / 1'000'000;

                response += "Processing time: "
                            + std::to_string(elapsed_ms) + " ms | "
                            + std::to_string(elapsed_us) + " us | "
                            + std::to_string(elapsed_ns) + " ns\r\n";

                if (!send_all(client_socket, response.c_str(), static_cast<int>(response.size()))) {
                    closesocket(client_socket);
                    return;
                }
            }
        }
    }

    SOCKET create_server_socket(int port)
    {
        const SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_socket == INVALID_SOCKET) {
            std::cerr << "socket() failed.\n";
            return INVALID_SOCKET;
        }

        constexpr BOOL opt = TRUE;
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(static_cast<u_short>(port));
        server_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
            std::cerr << "bind() failed.\n";
            closesocket(server_socket);
            return INVALID_SOCKET;
        }

        if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "listen() failed.\n";
            closesocket(server_socket);
            return INVALID_SOCKET;
        }

        return server_socket;
    }

    [[noreturn]] void run_server(SOCKET server_socket, int port)
    {
        std::cout << "TCP Server listening on port " << port << "...\n";

        while (true) {
            sockaddr_in client_addr{};
            int client_size = sizeof(client_addr);

            const SOCKET client_socket = accept(
                    server_socket, reinterpret_cast<sockaddr*>(&client_addr),
                    &client_size);

            if (client_socket == INVALID_SOCKET) {
                std::cerr << "accept() failed, continuing...\n";
                continue;
            }

            std::thread t(handle_client, client_socket);
            t.detach();
        }
    }
} // namespace tcpp
