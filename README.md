# telnet_tcpp

Minimal multi-client TCP command server written in **C++20** using **Winsock2**.

Experimental TCP command server featuring persistent connections,
a simple custom command protocol (PING / TIME / ECHO), and static Windows builds.

## Features

- multi-client threaded TCP server
- persistent connections
- simple command protocol:
    - `PING` → `PONG`
    - `TIME` → server time
    - `ECHO <text>`
    - `EXIT`
- static Windows release builds

Tested on:

`Windows 11 + MinGW`

## Usage

Run server (default port 10000):

`telnet_tcpp.exe`

Run server on a custom port:

`telnet_tcpp.exe 5555`

Connect:

`telnet localhost 10000`
