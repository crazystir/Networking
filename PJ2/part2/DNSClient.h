//
// Created by gqt on 4/26/18.
//

#ifndef PJ2_DNSCLIENT_H
#define PJ2_DNSCLIENT_H

#include "client.h"

using namespace std;

class DNSClient : public client{
public:
    DNSClient();
    DNSClient(tran_type, char*, u_int);
    void execute(string&) override ;
    void setContent(string);
    string getContent();
private:
    string content;
};


#endif //PJ2_DNSCLIENT_H
