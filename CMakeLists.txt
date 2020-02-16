cmake_minimum_required(VERSION 3.12)
project(httpserv C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_EXE_LINKER_FLAGS "-lm")

find_library(mhash REQUIRED)

add_executable(httpserv main.c tcpserver.h tcpserver.c client.h errors.h errors.c client.c json.c json.h)
target_link_libraries(httpserv PUBLIC mhash)