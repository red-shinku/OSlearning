#include "TcpServer.h"
#include "webhandler.h"
#include "error.h"

int main(int argc, char* argv[])
{
    if(argc != 2)
        err_sys("usage: runtcps [port]");
    int port = atoi(argv[1]);

    TcpServer *server = new TcpServer(port);
    Handler *web = new WebHandler("./");
    server->init(web);
    server->run();
    
    delete server;
}
