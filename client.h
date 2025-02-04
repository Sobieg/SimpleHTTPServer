/*
    Copyright (c) Евгений Крамсаков.
    СПбПУ, ВШКиЗИ, 2020
*/

#ifndef HTTPSERV_CLIENT_H
#define HTTPSERV_CLIENT_H
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include "mhash.h"

#define RCV_BUFFER_SIZE (BUFFER_SIZE+512)
#define BUFFER_SIZE 2048
#define SHA512_SIZE 64
#define GOST_SIZE 32

typedef struct cli {
    struct sockaddr_in addr;
    int cs;
    unsigned char buff[RCV_BUFFER_SIZE];
    unsigned char data[BUFFER_SIZE];
    unsigned char sha512_hash[SHA512_SIZE];
    unsigned char gost_hash[GOST_SIZE];
    unsigned char shabuff[SHA512_SIZE*2+1];
    unsigned char gostbuff[GOST_SIZE*2+1];
    int is_error;
} client;

typedef struct clients_list {
    client* client;
    struct clients_list* next;
    struct clients_list* prev;

} clients;

/**
 * Inits clients list
 * @param clients list to init
 * @return pointer to list
 */
clients* init_client_list(clients* cls);

/**
 * Add client to the tail of the list
 * @param clients list of clients to add
 * @param cli client to add
 * @return pointer to client
 */
clients* add_new_client(clients* cls, client* cli);

/**
 * Remove client from list
 * @param cli client to remove
 * @return pointer to the client after cli
 */
clients* remove_client(clients* cli);

#endif //HTTPSERV_CLIENT_H
