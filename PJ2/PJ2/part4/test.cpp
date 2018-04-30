#include <stdio.h>
#include <string.h>

#ifdef WIN32
    #include <windows.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <arpa/inet.h>
    #include <thread>
#endif

using namespace std;

class TcpThread{ // 多线程类
public:
        void Main(){ // 每一个线程的入口函数
            char buf[1024] = {0};
            for(;;){
                int lenRecv = recv(client,buf, sizeof(buf)-1,0); // server读取client端键入的数据(第二类socket的句柄，存储数据的地方,flag)
                if(lenRecv <= 0) break;
                buf[lenRecv] = '\0'; // 客户端键入数据的末尾两位赋值‘、0’
                if(strstr(buf,"quit")!= NULL){
                    break; // 字符串匹配函数，匹配到用户发送了"quit"
                }
                printf("Recvd data: %s \n Len: %d \n", buf, lenRecv); // 服务器显示客户端键入的字符串长度
            }

            #ifdef WIN32 // 读取数据的第二类socket创建后要记得关闭
                closesocket(client);
            #else
                close(client);       
            #endif

            delete this; // 调用完后，自己清理调第二类socket的对象
        }

        int client;// 每一个客户端的第二类socket
};


int main(int argc, char *argv[]){

    // 初始化”ws2_32.lib”
    #ifdef WIN32
        WSADATA ws;
        WSAStartup(MAKEWORD(2,2), &ws);
    #endif

    // 创建第一类socket
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock == -1){
        printf("create sock error!\n");
        return -1;
    }

    // TCP Server指定端口并设置服务端端口的属性，返回(sockaddr*)&saddr

    unsigned short port = 53; // 默认端口号
    if(argc > 1){
        port = atoi(argv[1]);
    }
    sockaddr_in saddr; // 声明端口
    saddr.sin_family = AF_INET; // TCPIP协议
    saddr.sin_port = htons(port); // 绑定端口号, htons()之host-to-network
    saddr.sin_addr.s_addr = 0; //或htonl(0) 服务器接受的IP地址 0表示接受任意内外网IP

    // 绑定端口到指定的socket，输入(sockaddr*)&saddr
    if(bind(sock, (sockaddr*)&saddr, sizeof(saddr))!=0){
        printf("OS bind socks to this port %d failed\n", port);
        return -2;
    }
    printf("OS bind this port %d to sockets successfully!\n", port);
    listen(sock, 10); // 允许用户连接函数（客户socket（一个客户一个socket），最大请求数队列的长度，）



    for(;;){ // 支持多个客户端第二类socket
        sockaddr_in caddr; // 结构体：存储客户端的相关信息:端口号和IP地址s
        socklen_t len = sizeof(caddr); 
        int client = accept(sock,(sockaddr*)&caddr,&len); // 第二类socket: 创建一个socket专门读取缓冲区clients（这里缓冲区大小如上行listen代码所示为10）
        if(client<=0)break;
        printf("accept client %d", client);
        char *ip = inet_ntoa(caddr.sin_addr); // 客户端IP地址转字符串
        unsigned short cport = ntohs(caddr.sin_port);// 客户端端口号（网络字节序转本地字节序）
        printf("client ip: %s, port is %d\n", ip, cport); // 打印客户端连接信息

        TcpThread *th = new TcpThread(); // 创建第二类socket对象
        th->client = client; // 传递client到TcpThread对象
        thread sth(&TcpThread::Main,th); // 启动线程的入口函数main(),参数为thread，函数库为thread sth()
        sth.detach();
           }


    #ifdef WIN32 // 端口的第一类socket，不再交互后也要记得关闭,先二后一时堆栈思想
        closesocket(sock);
    #else
        close(sock);
    #endif



    getchar();
    return 0;
}
