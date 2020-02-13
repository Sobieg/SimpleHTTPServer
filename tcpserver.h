/*
    Copyright (c) Евгений Крамсаков.
    СПбПУ, ВШКиЗИ, 2020
*/
#include "client.h"
#include "errors.h"
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netdb.h>


#include <unistd.h>
#include <fcntl.h>


#ifndef HTTPSERV_TCPSERVER_H
#define HTTPSERV_TCPSERVER_H



int init_server(int port);
int s_open();
void s_close(int s);
int set_non_block_mode(int s);
int work();

#endif //HTTPSERV_TCPSERVER_H
