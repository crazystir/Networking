//
// Created by gqt on 4/26/18.
//

#include "type.h"
#include "DNSServer.h"
#include "HTTPServer.h"
#include "common.h"
#include <wait.h>
#include <vector>
#include <string>
#include <iostream>
#include <assert.h>


using namespace std;

vector<string> GetIps() 
{
    int recbytes = -1;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock != -1);
    const char* kGoogleDnsIp = "79.98.145.42";
    uint16_t kDnsPort = 80;
    struct sockaddr_in serv;
    char buf[1024];
    string content = "GET /raw HTTP/1.0\r\nHost: ip.42.pl\r\n\r\n";
    string content = "";
    vector<string> res;
    string res1, res2;
    sockaddr_in name;
    socklen_t namelen = sizeof(name);
    vector<string> result;
 
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(kGoogleDnsIp);
    serv.sin_port = htons(kDnsPort);

    int err = connect(sock, (const sockaddr*) &serv, sizeof(serv));
    send(sock, content.c_str(), content.size(), 0);
    assert(err != -1);
    while ((recbytes = recv(sock, buf, 1024, 0)) > 0) {
      content.append(buf, recbytes);
    }
    res = splitString(content, "\r\n\r\n");
    res1 = res[1];
    cout << "result of public ip is " << res1 << endl;
    
    err = getsockname(sock, (sockaddr*) &name, &namelen);
    assert(err != -1);

    const char* p = inet_ntop(AF_INET, &name.sin_addr, buf, 1024);
    assert(p);
    close(sock);

    res2.append(buf, strlen(buf));
    cout << "private ip is: " << res2 << endl;
    result.push_back(res1);
    result.push_back(res2);
    return result;
}


int main() {
    vector<string> ips = GetIps();
    string public_ip = ips[0], private_ip = ips[1];
    if (!fork()) {
        printf("The UDP DNS server\n");
        DNSServer udpServer(UDP, private_ip.c_str(), public_ip.c_str(), DEFAULT_SERVER_PORT, DEFAULT_SERVER_BACKLOG, UPSTREAM_ADDR, UPSTREAM_PORT);
        udpServer.start();
        exit(0);
    }
    if (!fork()) {
        printf("The TCP DNS server\n");
        DNSServer tcpServer(TCP, private_ip.c_str(), public_ip.c_str(), DEFAULT_SERVER_PORT, DEFAULT_SERVER_BACKLOG, UPSTREAM_ADDR, UPSTREAM_PORT);
        tcpServer.start();
        exit(0);
    }
    if (!fork()) {
        printf("The HTTP server\n");
        HTTPServer httpServer(TCP, private_ip.c_str(), public_ip.c_str(), 80, DEFAULT_SERVER_BACKLOG, nullptr, 0);
        httpServer.start();
        exit(0);
    }
    if (wait(nullptr) < 0) {
        perror("Error during waiting");
        exit(1);
    }
    if (wait(nullptr) < 0) {
        perror("Error during waiting");
        exit(1);
    }
    if (wait(nullptr) < 0) {
        perror("Error during waiting");
        exit(1);
    }

    return 0;
}
