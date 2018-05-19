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
    DNSServer(tran_type, char*, u_int, int, u_int, char*, u_int);
    void run(int) override ;

private:
    DNSClient dnsClient;
    char upstream_addr[40];
    u_int upstream_port;
};


#endif //PJ2_DNSSERVER_H
