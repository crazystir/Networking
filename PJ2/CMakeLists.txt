cmake_minimum_required(VERSION 3.9)
project(PJ2)

set(CMAKE_CXX_STANDARD 11)

add_executable(PJ2 PJ2/server.h PJ2/server.cpp PJ2/DNSServer.cpp PJ2/DNSServer.h PJ2/client.cpp PJ2/client.h PJ2/type.h PJ2/DNSClient.cpp PJ2/DNSClient.h PJ2/server_test.cpp)