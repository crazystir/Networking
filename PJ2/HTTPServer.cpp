//
// Created by gqt on 4/29/18.
//

#include "HTTPServer.h"
#include "common.h"
#include <map>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

#define FILE_PATH "./error.html"
#define HOST "Host"

HTTPServer::HTTPServer() : server() {}

HTTPServer::HTTPServer(tran_type type, char * addr, u_int port, int backlog, char* out_addr
        , char* upstream_addr, u_int upstream_port) : server(type, addr, port, backlog, out_addr) {}


int HTTPServer::check_error(const string &) {}

void HTTPServer::error_handle(string &) {}

void HTTPServer::run_udp(int) {}

string getFile() {
    string response = "";
    string tmp;
    fstream file;

    file.open(FILE_PATH, std::fstream::in);
    while (getline(file, tmp)) {
        response.append(tmp);
    }
    file.close();
    return response;

}

int parseHTML(const string& content, map<string, string>& res) {
    try {
        vector<string> strings = splitString(content, "\r\n");
        strings.erase(strings.begin());
        vector<string> tmp;

        for (string s : strings) {
            if (s.empty())
                break;
            tmp = splitString(s, ": ");
            res[tmp[0]] = tmp[1];
        }
        return 0;
    } catch (exception& e) {
        perror("Error during parse HTML header.\n");
        return -1;
    }

};

void HTTPServer::run_tcp(int client_fd, struct sockaddr * remote_addr) {
    int recbytes = -1;
    char buf[MAX_DATA_SIZE];
    string content = "";
    string response = "";
    string domainName = "Not known";
    map<string, string> context;

    memset(buf, 0, sizeof(buf));
    while ((recbytes = recv(client_fd, buf, MAX_DATA_SIZE, 0)) > 0) {
        content.append(buf, recbytes);
        if (content.size() >= 4 && content.substr(content.size() - 4, 4) == "\r\n\r\n") {
            break;
        }
    }
    if (recbytes < 0) {
        perror("Error during receiving data in HTML server\n");
        return;
    }
    printf("%s\n", content.c_str());
    if ((parseHTML(content, context)) < 0) {
        return;
    }
    if (context.find(HOST) != context.end()) {
        domainName = context[HOST];
    }

    response = getFile();
    response.replace(response.find("{{domain}}"), 10, domainName);
    send(client_fd, response.c_str(), response.size(), 0);
}
