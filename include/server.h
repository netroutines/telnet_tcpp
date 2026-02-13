#pragma once
#include <winsock2.h>

namespace tcpp {
    SOCKET create_server_socket(int port);
    [[noreturn]] void run_server(SOCKET server_socket, int port);
} // namespace tcpp
