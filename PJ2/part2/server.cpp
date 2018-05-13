//
// Created by gqt on 4/26/18.
//

#include "server.h"



server::server() {
    port = DEFAULT_SERVER_PORT;
    strncpy((char*)addr, DEFAULT_ADDR, 40);
    type = DEFAULT_SERVER_TYPE;
    backlog = DEFAULT_SERVER_BACKLOG;
    init();
}

server::server(tran_type type, char* addr , u_int port, int backlog) {
    this->type = type;
    strncpy(this->addr, addr, 40);
    this->port = port;
    this->backlog = backlog;
    init();
}

void server::init() {
    long flag = 1;
    struct sockaddr_in server_addr;
    __socket_type stream_type = type == UDP ? SOCK_DGRAM : SOCK_STREAM;
    if ((socket_fd = socket(AF_INET, stream_type, 0)) == -1) {
        perror("Socket error during creating the accept socket");
        exit(1);
    }
    printf("%d %d\n", socket_fd, port);
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(flag));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((uint16_t )port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&(server_addr.sin_zero), 8);
    if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("Error during binding the accept socket");
        exit(1);
    }
}

void server::execute() {
    printf("Execute\n");
    while (true) {
        //If the type is UDP, we move forward to udp handler directly beacuse UDP doesn't have accpet part.
        run(socket_fd);
    }
}

void server::finish() {
    printf("Finish server socket\n");
    close(socket_fd);
}

void server::start() {
    execute();
    finish();
}
