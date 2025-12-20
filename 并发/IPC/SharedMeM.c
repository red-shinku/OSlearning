#include "SharedMeM.h"

//FIXME: 初始化时如何锁？
int create_shm(const char* _key, off_t size)
{
    int fd = shm_open(_key, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, size);
    return fd;
}

void create_shmhead(void* pshm)
{
    ShmHead* pshmhead = (ShmHead*)pshm;
    pthread_mutex_init(&pshmhead->rlock, NULL);
    pthread_mutex_init(&pshmhead->wlock, NULL);
    sem_init(&pshmhead->rmtx, 1, 1);
    sem_init(&pshmhead->wmtx, 1, 1);
    //pshmhead->rmtx = sem_open("/readmtx", O_CREAT, 0666, 1);
    //pshmhead->wmtx = sem_open("/writemtx", O_CREAT, 0666, 1);

    pshmhead->writers = 0; //FIXME: or 1?
    pshmhead->readers = 0;
    for(int i = 0; i < MAX_BLOCKS; ++i)
    {
        pshmhead->offsets[i] = 0;
    }
}

Shmem shm_init()
{
    int fd = create_shm(Shm_T_NAME, 4096);
    void* pshm =  mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    create_shmhead(pshm);

    Shmem shmem;
    shmem.fd = fd;
    shmem.pshm = pshm;
    return shmem;
}

Shmem open_shm(const char* _key)
{
    int fd = shm_open(_key, O_CREAT | O_RDWR, 0666);
    void* pshm = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    Shmem shmem;
    shmem.fd = fd;
    shmem.pshm = pshm;
    return shmem;
}

//读者写者各有一把锁，且都交由对方控制。
//区别是读者在登场就需要该锁，写者只在实际写数据才需要。
//所以当有写者登场，之后的读者便无法出现
void will_read(void* pshm)
{
    ShmHead* pshmh = (ShmHead*)pshm;
    sem_wait(&pshmh->rmtx);

    ++pshmh->readers;
    if(pshmh->readers == 1);
        sem_wait(&pshmh->wmtx);

    sem_post(&pshmh->rmtx);
}

void finish_read(void* pshm)
{
    ShmHead* pshmh = (ShmHead*)pshm;
    pthread_mutex_lock(&pshmh->rlock);

    --pshmh->readers;
    if(pshmh->readers == 0)
        sem_post(&pshmh->wmtx);
    
    pthread_mutex_unlock(&pshmh->rlock);
}

void will_write(void* pshm)
{
    ShmHead* pshmh = (ShmHead*)pshm;
    pthread_mutex_lock(&pshmh->wlock);

    ++pshmh->writers;
    if(pshmh->writers == 1)
        sem_wait(&pshmh->rmtx);

    pthread_mutex_unlock(&pshmh->wlock);

    sem_wait(&pshmh->wmtx);
}

void finish_write(void* pshm)
{
    ShmHead* pshmh = (ShmHead*)pshm;
    sem_post(&pshmh->wmtx);

    pthread_mutex_lock(&pshmh->wlock);

    --pshmh->writers;
    if(pshmh->writers == 0)
        sem_post(&pshmh->rmtx);

    pthread_mutex_unlock(&pshmh->wlock);
    
}

