#include <time.h>
#include <unistd.h>
#define  __USE_GNU
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    if(argc != 2){
        perror("[ERROR] need two args\n");
        return -1;
    }

    int total = atoi(argv[1]);

    int pipefd[2];
    int pipefd2[2];
    if(pipe(pipefd) == -1){
        perror("[ERROR] failed to create pipefd\n");
        return -1;
    }
    if(pipe(pipefd2) == -1){
        perror("[ERROR] failed to create pipefd2\n");
        return -1;
    }

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);

    //create child process and work on the same cpe with father
    int pid = fork();

    if(pid < 0){
        perror("[ERROR] failed to fork\n");
        return -1;
    }
    else if(pid == 0){
        sched_setaffinity(getpid(), sizeof(cpuset), &cpuset);
        //child process use to read and cacullate the using time
        close(pipefd[1]);
        close(pipefd2[0]);

        struct timespec start, end;
        double totaltime = 0;

        int tryc = 1;
        while(tryc <= total){
            read(pipefd[0], &start, sizeof(start));
            clock_gettime(CLOCK_MONOTONIC, &end);
            
            double usetime = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_nsec - start.tv_nsec) / 1e3;
            totaltime += usetime;
            //printf("%d: [pid %d]read usetime: %lfus \n", tryc, getpid(), usetime);
            
            ++tryc;
            write(pipefd2[1], &tryc, sizeof(tryc));
        }
        printf("\n  complete testing...average usetime: %lfus \n", totaltime / total);

        close(pipefd[0]);
        close(pipefd2[1]);
        exit(EXIT_SUCCESS);
    }
    else{
        sched_setaffinity(getpid(), sizeof(cpuset), &cpuset);
        //father process use to write
        close(pipefd[0]);
        close(pipefd2[1]);

        struct timespec start;

        int tryc = 1;
        while(tryc <= total){
            //printf("%d: [pid %d]write\n", tryc, getpid());
            clock_gettime(CLOCK_MONOTONIC, &start);
            write(pipefd[1], &start, sizeof(start));
            read(pipefd2[0], &tryc, sizeof(tryc));
        }

        close(pipefd[1]);
        close(pipefd2[0]);
        exit(EXIT_SUCCESS);
    }

    return 0;
}