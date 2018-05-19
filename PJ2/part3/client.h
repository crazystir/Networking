//
// Created by gqt on 4/26/18.
//

#ifndef PJ2_CLIENT_H
#define PJ2_CLIENT_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "type.h"
#include <string>

using namespace std;

class client {
public:
    client();
    client(tran_type, char [], u_int);
    void launch(char [], u_int, string&);
    void init();
    void conn(char [], u_int);
    virtual void execute(string&) = 0;
    void finish();
protected:
    int client_fd;
    tran_type type;
    char addr[40];
    u_int port;
    sockaddr_in remote_addr;


};


#endif //PJ2_CLIENT_H
