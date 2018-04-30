CC=g++
PWD=$(shell pwd)
FLAG=-std=c++11

all: server client dnsclient dnsserver test

server: server.h server.cpp
	$(CC) $(FLAG) -c server.h server.cpp 

client: client.h client.cpp
	$(CC) $(FLAG) -c client.h client.cpp

dnsclient: DNSClient.h DNSClient.cpp
	$(CC) $(FLAG) -c DNSClient.h DNSClient.cpp

dnsserver: DNSServer.h DNSServer.cpp
	$(CC) $(FLAG) -c DNSServer.h DNSServer.cpp 
test: *.o server_test.cpp 
	$(CC) $(FLAG) *.o server_test.cpp -o test

