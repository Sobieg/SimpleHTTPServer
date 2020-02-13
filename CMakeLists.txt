cmake_minimum_required(VERSION 3.12)
project(httpserv C)

set(CMAKE_C_STANDARD 99)

add_executable(httpserv main.c tcpserver.h tcpserver.c client.h errors.h errors.c)