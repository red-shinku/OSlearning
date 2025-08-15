//add -lrt when gcc
#include <time.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("need two args\n");
        return -1;
    }

    int total = atoi(argv[1]);
    double total_usetime = 0;

    for(int i=0; i<total; ++i){
        
        struct timespec start, end;

        clock_gettime(CLOCK_MONOTONIC, &start);
        syscall(SYS_getpid);
        clock_gettime(CLOCK_MONOTONIC, &end);

        double usetime = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_nsec - start.tv_nsec) / 1e3;

        printf("%d: try syscall usetime: %lfus \n", i+1, usetime);
        total_usetime += usetime;
    }

    printf("\n average: usetime: %lfus \n", total_usetime/total);
    
    return 0;
}