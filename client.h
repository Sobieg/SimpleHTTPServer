/*
    Copyright (c) Евгений Крамсаков.
    СПбПУ, ВШКиЗИ, 2020
*/

#ifndef HTTPSERV_CLIENT_H
#define HTTPSERV_CLIENT_H
#include <netdb.h>

typedef struct cli {
    struct sockaddr_in addr;
    int cs;
} client;

#endif //HTTPSERV_CLIENT_H
