//
// Created by gqt on 4/26/18.
//

#include "DNSServer.h"
#include "HTTPServer.h"
#include <wait.h>

using namespace std;

int main() {
    if (!fork()) {
        printf("The UDP DNS server\n");
        DNSServer udpServer;
        udpServer.start();
        exit(0);
    }
    if (!fork()) {
        printf("The TCP DNS server\n");
        DNSServer tcpServer(TCP, (char*)"10.105.120.129", 53, 5, (char*)"8.8.8.8", 53);
        tcpServer.start();
        exit(0);
    }
    if (!fork()) {
        printf("The HTTP server\n");
        HTTPServer httpServer(TCP, (char*)"10.105.120.129", 80, 5, nullptr, 0);
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
