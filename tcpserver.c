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
        return sock_err("bind", listening_socket);
    }

    if (listen(listening_socket, 1) < 0) {
        return sock_err("listen", listening_socket);
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
    return s;
}

void s_close(int s) {
    close(s);
}

int set_non_block_mode(int s){
    int fl = fcntl(s, F_GETFL, 0);
    return fcntl(s, F_SETFL, fl | O_NONBLOCK);
}
