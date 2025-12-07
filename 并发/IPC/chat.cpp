#include <iostream>
#include "SharedMeM.h"

class Chat
{
public:
    Chat();
    ~Chat();

    void run();

private:
    char mode;
    int fd;
    void* pshm;
    int* pint;

    void write();
    void read();
    void openshm();
};

Chat::Chat():
pshm(NULL), pint(NULL)
{ }

Chat::~Chat()
{
    munmap(pshm, 4096);
    close(fd);
    if(mode == 'w')
    shm_unlink(SEM_T_NAME);
}

void Chat::run()
{
    std::cin >> mode;
    openshm();
    pint = (int*)(pshm + sizeof(ShmHead));
    if(mode == 'w')
        write();
    else if(mode == 'r')
        read();
    else
        std::cout << "oops" << std::endl;
}

void Chat::write()
{
    int data;
    while (true)
    {
        std::cin >> data;
        will_write(pshm);
        *pint = data;
        finish_write(pshm);
    }
}

void Chat::read()
{
    while (true)
    {
        will_read(pshm);
        std::cout << *pint << std::endl;
        finish_read(pshm);
    }
}

void Chat::openshm()
{
    if(mode == 'w')
    {
        auto shmem = shm_init();
        fd = shmem.fd;
        pshm = shmem.pshm;
    }
    else if(mode == 'r')
    {
        auto shmem = open_shm(SEM_T_NAME); 
        fd = shmem.fd;
        pshm = shmem.pshm;
    }
}

int main()
{
    Chat chat;
    chat.run();
}