#include <sys/ipc.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define SEM_T_NAME "/mysem"

typedef struct SharedListnode
{
    int data;
    off_t next;
}ShListnode;

typedef struct ShmMux
{
    pthread_mutex_t mtx;
    sem_t *write;

}ShmMux;

typedef struct ShmDetail
{
    int fd;
    ShmMux* shmux;
}ShmDetail;


int create_shm(const char* _key, off_t size)
{
    int fd = shm_open(_key, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, size);
    return fd;
}

//FIXME: 改为直接写入共享内存？
ShmMux* create_shmmux()
{
    ShmMux *shmmux = (ShmMux*)malloc(sizeof(shmmux));
    pthread_mutex_init(&shmmux->mtx, NULL);
    shmmux->write = sem_open(SEM_T_NAME, O_CREAT, 0666, 1);
    return shmmux;
}

//FIXME: 返回ShmDetail
void* shm_init()
{
    return mmap(NULL, , PROT_READ | PROT_WRITE, fd, 0);
}

ShListnode* new_listnode(int data)
{
    ShListnode *node = (ShListnode*)malloc(sizeof(node));
    node->data = data;
    return node;
}


