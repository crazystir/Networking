//
// Created by gqt on 4/26/18.
//

#include "type.h"
#include "DNSServer.h"
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
    string request= "GET /raw HTTP/1.0\r\nHost: ip.42.pl\r\n\r\n";
    string response= "";
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
    send(sock, request.c_str(), request.size(), 0);
    assert(err != -1);
    while ((recbytes = recv(sock, buf, 1024, 0)) > 0) {
      response.append(buf, recbytes);
    }
    res = splitString(response, "\r\n\r\n");
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
    char public_ip[40], private_ip[40];
    strncpy(public_ip, ips[0].c_str(), 40);
    strncpy(private_ip, ips[1].c_str(), 40);
    if (!fork()) {
        printf("The UDP DNS server\n");
        DNSServer udpServer(UDP, private_ip, DEFAULT_SERVER_PORT, 10, 8080, (char*)UPSTREAM_ADDR, UPSTREAM_PORT);
        udpServer.start();
        exit(0);
    }
    if (!fork()) {
        printf("The TCP DNS server\n");
        DNSServer tcpServer(TCP, private_ip, DEFAULT_SERVER_PORT, 10, 8081, (char*)UPSTREAM_ADDR, UPSTREAM_PORT);
        tcpServer.start();
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

    return 0;
}
