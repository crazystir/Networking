//
// Created by gqt on 4/26/18.
//

#include "DNSServer.h"
#include <iostream>

using namespace std;


DNSServer::DNSServer() : server(){
    dnsClient = DNSClient();
    strncpy(upstream_addr, UPSTREAM_ADDR, 40);
    upstream_port = UPSTREAM_PORT;
}

DNSServer::DNSServer(tran_type type, char* addr, u_int port, int backlog, u_int c_port
        , char* upstream_addr, u_int upstream_port) : server(type, addr, port, backlog) {
    dnsClient = DNSClient(type, addr, c_port);
    strncpy(this->upstream_addr, upstream_addr, 40);
    this->upstream_port = upstream_port;
}

void DNSServer::run_udp(int socket_fd) {
    char buf[MAX_DATA_SIZE];
    int recvbypes;
    string content = "";
    string result = "";
    sockaddr_in remote_addr;
    socklen_t len = sizeof(remote_addr);

    memset(&remote_addr, 0, sizeof(remote_addr));
    memset(buf, '\0', sizeof(buf));
    recvbypes = recvfrom(socket_fd, buf, MAX_DATA_SIZE, 0, (struct sockaddr*) &remote_addr, &len);
    if (recvbypes < 0) {
        perror("Error during receving udp socket\n");
        return;
    }
    content.append(buf, recvbypes);
    dnsClient.setContent(content);
    dnsClient.launch(upstream_addr, upstream_port, result);
    if (sendto(socket_fd, result.c_str(), result.size(), 0, (struct sockaddr*) &remote_addr, len) < 0) {
        perror("Server error during sending socket back to client");
        return;
    }
}

void DNSServer::run_tcp(int client_fd, struct sockaddr * remote_addr) {
    char buf[MAX_DATA_SIZE];
    int recvbypes;
    string content = "";
    string result = "";

    memset(buf, 0 , sizeof(buf));
    recvbypes = recv(client_fd, buf, MAX_DATA_SIZE, 0);
    if (recvbypes < 0) {
        perror("Server error during receving socket");
        return;
    }
    content.append(buf, recvbypes);
    dnsClient.setContent(content);
    dnsClient.launch(upstream_addr, upstream_port, result);
    if (send(client_fd, result.c_str(), result.length(), 0) < 0) {
        perror("Server error during sending socket back to client");
        return;
    }
}
