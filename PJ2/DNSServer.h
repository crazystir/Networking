//
// Created by gqt on 4/26/18.
//

#ifndef PJ2_DNSSERVER_H
#define PJ2_DNSSERVER_H


#include "server.h"
#include "DNSClient.h"
#include <string>


using namespace std;

class DNSServer : public server{
public:
    DNSServer();
    DNSServer(tran_type, char*, u_int, int, u_int, char*, char*, u_int);
    inline int check_error(const string&);
    void run_udp(int) override ;
    void run_tcp(int, struct sockaddr*) override ;
    void error_handle(string&) override;

private:
    DNSClient dnsClient;
    char upstream_addr[40];
    u_int upstream_port;

};


#endif //PJ2_DNSSERVER_H
