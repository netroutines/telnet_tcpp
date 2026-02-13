cmake_minimum_required(VERSION 4.1)
project(telnet_tcpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(telnet_tcpp source/main.cpp source/server.cpp)

target_include_directories(telnet_tcpp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# WinSock2
target_link_libraries(telnet_tcpp PRIVATE ws2_32)

# static runtime
target_link_options(telnet_tcpp PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
)
