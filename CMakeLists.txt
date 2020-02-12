cmake_minimum_required(VERSION 3.15)
project(httpserv C)

set(CMAKE_C_STANDARD 99)

add_executable(infotecs main.c tcpserver.h tcpserver.cpp)