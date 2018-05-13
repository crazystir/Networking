//
// Created by gqt on 4/26/18.
//

#ifndef PJ2_SERVER_H
#define PJ2_SERVER_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include "type.h"
#include <string>

using namespace std;

class server {
public:
    server();
    server(tran_type, char*, u_int, int);
    void start();
    void init();
    void execute();
    void finish();
    virtual void run(int) = 0;

protected:
    tran_type type;
    char addr[40];
    u_int port;
    int socket_fd;
    int backlog;


};

#endif //PJ2_SERVER_H
