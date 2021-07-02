cmake_minimum_required(VERSION 3.19)
project(ssl_SocketProgramming)

set(CMAKE_C_STANDARD 99)
add_executable(Server echo_mpserv.c)
add_executable(Client1 echo_client.c)
add_executable(Client2 echo_client.c)
