CC=g++
FLAG=-std=c++11 -Wall
CPP = common.cpp server.cpp client.cpp DNSClient.cpp DNSServer.cpp
HEADER = common.h server.h client.h DNSClient.h DNSServer.h

.PHONY: all
all: main

main: main.cpp $(CPP) $(HEADER) 
	$(CC) $(FLAG) $< $(CPP) $(HEADER) -o main

clean:
	rm main
