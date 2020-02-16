/*
    Copyright (c) Евгений Крамсаков.
    СПбПУ, ВШКиЗИ, 2020
*/


#include "tcpserver.h"



int listening_port = 8090;
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
    struct timeval tv = {0, 100};

    int time_to_exit = 0;

    while(!time_to_exit) {
        FD_ZERO(&rfd);
        FD_ZERO(&wfd);

        FD_SET(listening_socket, &rfd);

        int select_result = 0;

        clients* cur_cli = clients_list;
        while(cur_cli != 0) {
            FD_SET(cur_cli->client->cs, &rfd);
            FD_SET(cur_cli->client->cs, &wfd);
            if (nfds < cur_cli->client->cs) {
                nfds = cur_cli->client->cs;
            }
            cur_cli = cur_cli->next;
        }
        select_result = select(nfds+1, &rfd, &wfd, 0, &tv);
        if (select_result) {
            if (FD_ISSET(listening_socket, &rfd)) {
                // Handle all new connections
                client cl;
                memset(&cl, 0, sizeof(client));

                int addrlen = sizeof(cl.addr);
                cl.cs = accept(listening_socket, (struct sockaddr*) &cl.addr, (socklen_t*) &addrlen);

                if (cl.cs < 0) {
                    return sock_err("accept", cl.cs);
                }

                FD_SET(cl.cs, &rfd);
                FD_SET(cl.cs, &wfd);

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
                    clients_list_end = add_new_client(clients_list_end, &cl);
                }
            }
            cur_cli = clients_list;
            while (cur_cli != 0) {
                if (FD_ISSET(cur_cli->client->cs, &rfd)) {
                    int rcv = 0;
                    unsigned char* buff = cur_cli->client->buff;
                    rcv += (int) recv(cur_cli->client->cs, buff+rcv, RCV_BUFFER_SIZE-rcv, 0);
                    if (rcv == 0) { //client disconnected
                        FD_CLR(cur_cli->client->cs, &rfd);
                        FD_CLR(cur_cli->client->cs, &rfd);
#if !defined(NDEBUG)
                        printf("Client disconnected: %d.%d.%d.%d:%d\n",
                               (cur_cli->client->addr.sin_addr.s_addr >> 0)&0xff,
                               (cur_cli->client->addr.sin_addr.s_addr >> 8)&0xff,
                               (cur_cli->client->addr.sin_addr.s_addr >> 16)&0xff,
                               (cur_cli->client->addr.sin_addr.s_addr >> 24)&0xff,
                               htons(cur_cli->client->addr.sin_port)
                        );
#endif
                        s_close(cur_cli->client->cs);
                        if (cur_cli == clients_list) {
                            clients* tmp = clients_list;
                            clients_list = remove_client(cur_cli);
                            if (tmp == clients_list_end) {
                                clients_list_end = clients_list;
                            }
                            cur_cli = clients_list;
                        }
                        else if (cur_cli == clients_list_end){
                            clients_list_end = cur_cli->prev;
                            cur_cli = remove_client(cur_cli);
                        }
                        else {
                            cur_cli = remove_client(cur_cli);
                        }
                        continue;
                    }
                    const char* contenttipe = "Content-Type: ";
                    const char* applicationjson = "application/json";
                    const char* contentlenth = "Content-Length: ";
                    if (strstr(buff, applicationjson) != 0) {
                        char* startjson = strstr(buff, "{");
                        if (startjson != 0) {
                            json_value* val = json_parse(startjson, rcv);
                            if (!val) {
                                cur_cli->client->is_error = 1;
                            }
                            else {
                                memcpy(cur_cli->client->data, val->u.object.values->value->u.string.ptr,
                                       strlen(val->u.object.values->value->u.string.ptr));
                                MHASH sha512;
                                MHASH gost;
                                unsigned char *hash_sha512;
                                unsigned char *hash_gost;

                                sha512 = mhash_init(MHASH_SHA512);
                                if (sha512 == MHASH_FAILED) { exit(1); }
                                mhash(sha512, cur_cli->client->data, strlen(cur_cli->client->data));
                                hash_sha512 = mhash_end(sha512);
                                gost = mhash_init(MHASH_GOST);
                                if (gost == MHASH_FAILED) exit(1);
                                mhash(gost, cur_cli->client->data, strlen(cur_cli->client->data));
                                hash_gost = mhash_end(gost);
                                memcpy(cur_cli->client->sha512_hash, hash_sha512, SHA512_SIZE);
                                memcpy(cur_cli->client->gost_hash, hash_gost, GOST_SIZE);
                                cur_cli->client->is_error = 0;
                            }
                        }
                        else {
                            cur_cli->client->is_error = 1;
                        }
                    }
                    else {
                        cur_cli->client->is_error = 1;
                    }
                    //If data goes in multiople pieces
//                    while(buff[rcv] != '}') {
//                        if(rcv == RCV_BUFFER_SIZE) {
//                            //too many data
//                            break;
//                        }
//                        rcv += (int) recv(cur_cli->client->cs, buff+rcv, RCV_BUFFER_SIZE-rcv, 0);
//                    }
                }
                if (FD_ISSET(cur_cli->client->cs, &wfd)) {
                    int sent = 0;
                    int flags = MSG_NOSIGNAL;
                    int ret = 0;
//                    char sndbuff[RCV_BUFFER_SIZE] = {0};
                    if (cur_cli->client->is_error) {
                        sprintf(cur_cli->client->buff, "HTTP/1.0 404 Not Found\r\n\r\n <h1>404 Not Found </h1>");
                    }
                    else {
                        memset(cur_cli->client->buff, 0, RCV_BUFFER_SIZE);
                        for (int i = 0; i < SHA512_SIZE; i++) {
                            sprintf((cur_cli->client->shabuff) + i * 2, "%.2x", (cur_cli->client->sha512_hash)[i]);
                        }
                        for (int i = 0; i < GOST_SIZE; i++) {
                            sprintf((cur_cli->client->gostbuff) + i * 2, "%.2x", (cur_cli->client->gost_hash)[i]);
                        }
                        sprintf(cur_cli->client->buff,
                                "HTTP/1.0 200 OK\r\n\r\n{\r\n\"sha512\": \"%s\",\r\n\"gost\": \"%s\"\r\n}\r\n",
                                cur_cli->client->shabuff, cur_cli->client->gostbuff);
                    }
                    while (sent < strlen(cur_cli->client->buff)) {
                        ret = send(cur_cli->client->cs, cur_cli->client->buff + sent,
                                   strlen(cur_cli->client->buff) - sent, flags);
                        if (ret <= 0) {
                            return sock_err("send", cur_cli->client->cs);
                        }
                        sent += ret;
                    }
#if !defined(NDEBUG)
                    printf("Client disconnected: %d.%d.%d.%d:%d\n",
                           (cur_cli->client->addr.sin_addr.s_addr >> 0)&0xff,
                           (cur_cli->client->addr.sin_addr.s_addr >> 8)&0xff,
                           (cur_cli->client->addr.sin_addr.s_addr >> 16)&0xff,
                           (cur_cli->client->addr.sin_addr.s_addr >> 24)&0xff,
                           htons(cur_cli->client->addr.sin_port)
                    );
#endif
                    s_close(cur_cli->client->cs);
                    if (cur_cli == clients_list) {
                        clients* tmp = clients_list;
                        clients_list = remove_client(cur_cli);
                        if (tmp == clients_list_end) {
                            clients_list_end = clients_list;
                        }
                        cur_cli = clients_list;
                    }
                    else if (cur_cli == clients_list_end){
                        clients_list_end = cur_cli->prev;
                        cur_cli = remove_client(cur_cli);
                    }
                    else {
                        cur_cli = remove_client(cur_cli);
                    }
                    continue;
//                    char* answ[RCV_BUFFER_SIZE];
//                    memset(answ, 0, RCV_BUFFER_SIZE);
//                    unsigned char sha512[512];
//                    unsigned char gost[512];
//                    for (int i = 0; i < SHA512_SIZE; i++) {
//                        sprintf(sha512+i, "%.2x", cur_cli->client->sha512_hash[i]);
//                    }
//                    for (int i = 0; i<GOST_SIZE; i++) {
//                        sprintf(gost+i, "%.2x", cur_cli->client->gost_hash[i]);
//                    }
//                    sprintf(answ, "{\n\"sha512\": \"%s\"\n\"gost\": \"%s\"\n}", sha512, gost);
//                    printf(answ);
                }


                cur_cli = cur_cli->next;
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