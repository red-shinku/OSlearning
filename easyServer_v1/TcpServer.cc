#include "TcpServer.h"

#include <string.h>
#include <stdexcept>
#include "error.h"

TcpServer::TcpServer(int port):
handler(NULL)
{
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //handle all ip on this os
    servaddr.sin_port = htons(port);
}

TcpServer::~TcpServer()
{
    close(listenfd);
    delete handler;
}

void TcpServer::init(Handler *handler)
{
    try
    {
        this->handler = handler;
        tcpsv_socket();
        tcpsv_bind();
        tcpsv_listen();
    }
    catch(const std::exception& e)
    {
        err_sys(e.what());
    }
}

void TcpServer::run()
{
    try
    {
        while (true)
        {
            int connfd = tcpsv_accept();
            handler->handler(connfd);
        }
    }
    catch(const std::exception& e)
    {
        err_sys(e.what());
    }
}

void TcpServer::tcpsv_socket()
{
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw std::runtime_error("socket error");
}

void TcpServer::tcpsv_bind()
{
    if((bind(listenfd, reinterpret_cast<struct sockaddr*>(&servaddr), sizeof(servaddr))) < 0)
        throw std::runtime_error("bind error");
}

void TcpServer::tcpsv_listen()
{
    if((listen(listenfd, LISTENQ)) < 0)
        throw std::runtime_error("listen error");
}

int TcpServer::tcpsv_accept()
{
    int connfd = 0;
    if((connfd = accept(listenfd, NULL, NULL)) < 0)
        throw std::runtime_error("accept error");
    return connfd; 
}