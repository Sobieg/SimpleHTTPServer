/*
    Copyright (c) Евгений Крамсаков.
    СПбПУ, ВШКиЗИ, 2020
*/


#include "tcpserver.h"



int listening_port = 8080;
int listening_socket = 0;

/*
 * init server, return
 */
int init_server(int port) {

    listening_port = port ? port : 8080;

    listening_socket = s_open();

    struct sockaddr_in listen_addr;
    memset(&listen_addr, 0, sizeof(struct sockaddr_in));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(port);
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listening_socket, (struct sockaddr*) &listen_addr, sizeof(listen_addr)) < 0 ) {
        exit(sock_err("bind", listening_socket));
    }

    if (listen(listening_socket, 1) < 0) {
        exit(sock_err("listen", listening_socket));
    }

    set_non_block_mode(listening_socket);

    return listening_socket;
}

int s_open() {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s<0) {
        return sock_err("socket", s);
    }
    else{
        return s;
    }
}

void s_close(int s) {
    close(s);
}

int set_non_block_mode(int s){
    int fl = fcntl(s, F_GETFL, 0);
    return fcntl(s, F_SETFL, fl | O_NONBLOCK);
}

/**
 * infinite handle new clients
 */
int work() {
    clients* clients_list = 0;
    clients* clients_list_end = clients_list;
    fd_set rfd;
    fd_set wfd;
    int nfds = listening_socket;
    struct timeval tv = {1, 0};

    int time_to_exit = 0;

    while(!time_to_exit) {
        FD_ZERO(&rfd);
        FD_ZERO(&rfd);

        FD_SET(listening_socket, &rfd);

        int select_result = 0;

        clients* cur_cli = clients_list;
        while(cur_cli != 0) {
            FD_SET(cur_cli->client->cs, &rfd);
            FD_SET(cur_cli->client->cs, &wfd);
            if (nfds < cur_cli->client->cs) {
                nfds = cur_cli->client->cs;
            }
        }
        select_result = select(nfds+1, &rfd, &wfd, 0, &tv);
        if (select_result) {
            while (FD_ISSET(listening_socket, &rfd)) {
                // Handle all new connections
                client cl;
                memset(&cl, 0, sizeof(client));

                int addrlen = sizeof(cl.addr);
                cl.cs = accept(listening_socket, (struct sockaddr*) &cl.addr, (socklen_t*) &addrlen);

                if (cl.cs < 0) {
                    return sock_err("accept", cl.cs);
                }
#if !defined(NDEBUG)
                printf("Client connected: %d.%d.%d.%d:%d\n",
                       (cl.addr.sin_addr.s_addr >> 0)&0xff,
                       (cl.addr.sin_addr.s_addr >> 8)&0xff,
                       (cl.addr.sin_addr.s_addr >> 16)&0xff,
                       (cl.addr.sin_addr.s_addr >> 24)&0xff,
                       htons(cl.addr.sin_port)
                       );
#endif
                if (clients_list == 0) {
                    clients_list = calloc(1, sizeof(clients));
                    clients_list_end = clients_list;
                    clients_list->client = &cl;
                }
                else {
                    add_new_client(clients_list_end, &cl);
                }
            }
            cur_cli = clients_list;
            while (cur_cli != 0) {
                if (FD_ISSET(cur_cli->client->cs, &rfd)) {
                    //TODO: handle receiving new data from client
                }
                if (FD_ISSET(cur_cli->client->cs, &wfd)) {
                    //TODO: handle sending new data to the client
                }
            }
        }
        else if (select_result == -1) {
            printf("Error");
        }
        else {
            //nothing
        }
    }
    while(!clients_list) {
        clients_list = remove_client(clients_list);
    }
    return 0;
}