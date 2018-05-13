//
// Created by gqt on 4/29/18.
//

#ifndef PJ2_HTTPSERVER_H
#define PJ2_HTTPSERVER_H


#include "server.h"

using namespace std;

class HTTPServer : public server{
public:
    HTTPServer();
    HTTPServer(tran_type, char*, u_int, int, char*, char*, u_int);
    inline int check_error(const string&);
    void run_udp(int) override ;
    void run_tcp(int, struct sockaddr*) override ;
    void error_handle(string&) override;
};


#endif //PJ2_HTTPSERVER_H
