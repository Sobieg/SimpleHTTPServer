/*
    Copyright (c) Евгений Крамсаков.
    СПбПУ, ВШКиЗИ, 2020
*/

#include "client.h"

clients* init_client_list(clients* cls) {
    cls->next = 0;
    cls->prev = 0;
    memset(&(cls->client), 0, sizeof(client));
}

clients* add_new_client(clients* cls, client cli) {
    clients* old_next = cls->next;
    clients* new_cli = calloc(1, sizeof(clients));
    new_cli->client = cli;
    new_cli->next = old_next;
    new_cli->prev = cls;
    cls->next = new_cli;
    return new_cli;
}

clients* remove_client(clients* cli) {
    clients* next_cli = cli->next;
    if (cli->prev == 0) {
        memset(&(cli->client), 0, sizeof(client));
        cli->next = 0;
    }
    else {
        cli->prev = next_cli;
        free(cli);
    }
    return next_cli;
}