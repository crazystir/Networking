#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <arpa/inet.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

vector<string> splitString(const string& s, const string& c)
{
    vector<string> v;
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
    return v;
}

vector<string> GetPrimaryIp() 
{
    int recbytes = -1;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock != -1);
    const char* kGoogleDnsIp = "79.98.145.42";
    uint16_t kDnsPort = 80;
    struct sockaddr_in serv;
    char buf[1024];
    string content = "GET /raw HTTP/1.0\r\nHost: ip.42.pl\r\n\r\n";
    string __result = "";
    vector<string> res;
    string res1;
    sockaddr_in name;
    socklen_t namelen = sizeof(name);
    string res2;
    vector<string> result;
 
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(kGoogleDnsIp);
    serv.sin_port = htons(kDnsPort);

    int err = connect(sock, (const sockaddr*) &serv, sizeof(serv));
    send(sock, content.c_str(), content.size(), 0);
    assert(err != -1);
    while ((recbytes = recv(sock, buf, 1024, 0)) > 0) {
      __result.append(buf, recbytes);
    }
    res = splitString(__result, "\r\n\r\n");
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






int main(int argc, char const *argv[])
{
  GetPrimaryIp();
  cout << "string split test: "<< endl;
  string a = "Qingtong guo is DOG!";
  vector<string> res;
  res = splitString(a, " ");
  for (int i = 0; i < res.size(); i++){
    cout << res[i] << endl;
  } 



}
