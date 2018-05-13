//
// Created by gqt on 4/26/18.
//

#include "DNSServer.h"
#include <iostream>

using namespace std;

static int client_port = DEFAULT_CLIENT_PORT;


void printHex(const string &result) {
    int count = 0;
    cout << result.size() << endl;
    for (char c : result) {
        printf("%x ", c & 0xff);
    }
    cout << endl;
}

void printHex(const char * result, int length) {
    cout << length << endl;
    for (int i = 0; i < length; i++) {
        printf("%x ", result[i] & 0xff);
    }
    cout << endl;
}

static int DNSBeginOfAns(const string& result) {
    int index = 12;
    int i = (int)result[index];
    while (i != 0) {
        index += i + 1;
        i = (int)result[index];
    }
    return index + 5;
}

// Parse URL to binary in DNS protocol
static string parseURL(const string& url) {
    for (char c : url) {
        if ((c < '0' || c > '9') && c != '.') {
            perror("Not a valid IPV4 URL\n");
            exit(1);
        }
    }
    char result[4];
    ushort num = 0;
    int count = 0;
    for (char c : url) {
        if (c == '.') {
            result[count++] = (char)(num);
            num = 0;
            continue;
        }
        num = num * (short)10 + (short)(c - '0');
    }
    result[count] = (char)(num);
    return string(result);
}

DNSServer::DNSServer() : server(){
    dnsClient = DNSClient();
    strncpy(upstream_addr, UPSTREAM_ADDR, 40);
    upstream_port = UPSTREAM_PORT;
}

DNSServer::DNSServer(tran_type type, char* addr, u_int port, int backlog, u_int c_port, char* out_addr
        , char* upstream_addr, u_int upstream_port) : server(type, addr, port, backlog, out_addr) {
    printf("cliend port: %d\n", c_port);
    dnsClient = DNSClient(type, addr, c_port);
    strncpy(this->upstream_addr, upstream_addr, 40);
    this->upstream_port = upstream_port;
}


inline int DNSServer::check_error(const string& result) {
    // If there is an error number return -1. Otherwise return 0
    if (result[6] == 0 && result[7] == 0)
        return -1;
    return 0;
}

void DNSServer::error_handle(string& result) {
    char redirect[] = {(char)0xc0, (char)0x0c, (char)0x00, (char)0x01, (char)0x00, (char)0x01, (char)0x00, (char)0x00, (char)0x00, (char)0x7d, (char)0x00, (char)0x04};
    int ansIndex = DNSBeginOfAns(result);

    result[3] -= (result[3] & 15);
    result[7] = 1;
    result[8] = result[9] = 0;
    result = result.substr(0, ansIndex);
    result.append(redirect, sizeof(redirect));
    result.append(parseURL(out_addr));
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
    if (check_error(result) < 0) {
        printf("Invalid URL, so redirect to our DNS server\n");
        error_handle(result);
    }
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
    if (check_error(result) < 0) {
        printf("Invalid URL, so redirect to our DNS server\n");
        error_handle(result);
    }
    if (send(client_fd, result.c_str(), result.length(), 0) < 0) {
        perror("Server error during sending socket back to client");
        return;
    }
}
