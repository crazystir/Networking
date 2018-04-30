//
// Created by gqt on 4/26/18.
//

#include "client.h"

using namespace std;


client::client() {
    type = DEFAULT_CLIENT_TYPE;
    strncpy((char*)addr, DEFAULT_ADDR, 40);
    port = DEFAULT_CLIENT_PORT;
}

client::client(tran_type type, char addr[], u_int port) {
    this->type = type;
    strncpy((char*)this->addr, addr, 40);
    this->port = port;
}

void client::init() {
    client_fd = -1;
    struct sockaddr_in client_addr;
    long flag = 1;
    int stream_type = type == UDP ? SOCK_DGRAM : SOCK_STREAM;

    if ((client_fd = socket(AF_INET, stream_type, 0)) == -1) {
        perror("Error during creating client socket");
        exit(1);
    }
    setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(flag));
    client_addr.sin_addr.s_addr = inet_addr((char*)addr);
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons((uint16_t)port);
    if (bind(client_fd, (sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
        perror("Error during binding client socket");
        exit(1);
    }
}

void client::conn(char* remote_ip, u_int remote_port) {
    memset(&remote_addr, '\0', sizeof(remote_addr));
    remote_addr.sin_addr.s_addr = inet_addr(remote_ip);
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons((uint16_t)remote_port);
    bzero(&remote_addr.sin_zero, 8);
    if (connect(client_fd, (struct sockaddr*)&remote_addr, sizeof(remote_addr)) == -1) {
        perror("Connection error");
        exit(1);
    }
    printf("Connect to server (%s:%u) successfully\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
}

void client::finish() {
    printf("Finish client socket\n");
    close(client_fd);
}

void client::launch(char * remote_ip, u_int remote_port, string& result) {
    init();
    conn(remote_ip, remote_port);
    execute(result);
    finish();
}