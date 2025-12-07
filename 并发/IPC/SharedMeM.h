#include <sys/ipc.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define SEM_T_NAME "/mysem"  
#define MAX_BLOCKS 16  //max num of entries in ShmHead catalogue

//FIXME: 记录共享内存大小，防止越界？
typedef struct Shmem
{
    int fd;
    void* pshm;
}Shmem;

typedef struct ShmHead
{
    pthread_mutex_t rlock;
    pthread_mutex_t wlock;
    sem_t *rmtx;
    sem_t *wmtx;
    int writers;
    int readers;
    int offsets[MAX_BLOCKS];
}ShmHead;

int create_shm(const char* _key, off_t size);
void create_shmhead(void* pshm);
Shmem shm_init();
Shmem open_shm(const char* _key);
void will_read(void* pshm);
void finish_read(void* pshm);
void will_write(void* pshm);
void finish_write(void* pshm);
//TODO: 添加结构体的方法

