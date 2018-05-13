//
// Created by gqt on 4/26/18.
//

#include "DNSClient.h"
#include <iostream>

DNSClient::DNSClient():client() {}

DNSClient::DNSClient(tran_type type, char * addr, u_int port) : client(type, addr, port) {}


string DNSClient::getContent() {
    return content;
}

void DNSClient::setContent(string content) {
    this-> content = content;
}

void DNSClient::execute(string& result) {
    int recvbytes = -1;
    char buf[MAX_DATA_SIZE];

    result.clear();
    memset(buf, 0, sizeof(buf));
    if (send(client_fd, content.c_str(), content.size(), 0) < 0)  {
        perror("Error during sending socket");
        exit(1);
    }

    while ((recvbytes = recv(client_fd, buf, MAX_DATA_SIZE, 0)) > 0) {
        result.append(buf, recvbytes);
        if (type == UDP) {
            break;
        }
    }

    if (recvbytes < 0) {
        perror("Client error during receiving data");
        exit(1);
    }
}