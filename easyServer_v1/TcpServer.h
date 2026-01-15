#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <functional>
#include "Handler.h"

//以连接队列容量
#define LISTENQ 10

class TcpServer
{
public:
    TcpServer(int port);
    ~TcpServer();

    void init(Handler *handler);
    void run();

private:
    struct sockaddr_in servaddr;
    Handler *handler;
    int listenfd;

    void tcpsv_socket();
    void tcpsv_bind();
    void tcpsv_listen();
    int tcpsv_accept();
};

