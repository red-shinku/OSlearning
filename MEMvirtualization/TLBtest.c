#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdatomic.h>
#include <x86intrin.h>

//disrupt array
void shuffle(int *array, int size) {
    srand((unsigned int)time(NULL));

    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);         
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

double tlb(int PAGENUMS, long TRYTIMES, float CPU_MHz){

    //make sure work on the same cpu
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    sched_setaffinity(0, sizeof(cpuset), &cpuset);

    //linux : get your system's page size
    long PAGE_SIZE = sysconf(_SC_PAGESIZE);

    int jump = PAGE_SIZE/sizeof(int);
    int *index = (int*)malloc(PAGENUMS * sizeof(int));
    int *arr = (int*)malloc(PAGENUMS * jump * sizeof(int));
    if(arr == NULL){
        perror("failed to malloc array");
        return -1;
    }

    //memset(arr, 0, PAGENUMS * jump);
    for(int i=0; i < PAGENUMS * jump; i += jump){
        arr[i] = i;
    }
        //use index[] to visit arr randomly
    index[0] = 0;
    for(int i=1; i < PAGENUMS; ++i){
        index[i] = index[i-1] + jump;
    }

    shuffle(index, PAGENUMS);

    //test empty visit
    double emvisit_time;
    for(int j = 0; j < TRYTIMES; ++j){
        unsigned long long cstart, cend;
        unsigned int aux;
        
        unsigned int dummy;
        __asm__ __volatile__ ("cpuid" : "=a"(dummy) : "a"(0) : "%rbx", "%rcx", "%rdx"); 
        cstart = __rdtsc();

        for(int i = 0; i < PAGENUMS; ++i){

        }

        cend = __rdtscp(&aux);
        __asm__ __volatile__ ("cpuid" : "=a"(dummy) : "a"(0) : "%rbx", "%rcx", "%rdx");

        emvisit_time += (cend - cstart) * 1000 / CPU_MHz;
    }
    emvisit_time /= TRYTIMES;

    //start creating data
    double total_ustime = 0;

    //prevent compiler optimization
    volatile int *varr = arr; 

    //test TLB
    for(int j = 0; j < TRYTIMES; ++j){
        unsigned long long start, end;
        unsigned int aux;
        
        unsigned int dummy;
        __asm__ __volatile__ ("cpuid" : "=a"(dummy) : "a"(0) : "%rbx", "%rcx", "%rdx"); 
        start = __rdtsc();

        for(int i = 0; i < PAGENUMS; ++i){
            varr[ index[i] ]++;
        }

        end = __rdtscp(&aux);
        __asm__ __volatile__ ("cpuid" : "=a"(dummy) : "a"(0) : "%rbx", "%rcx", "%rdx");

        total_ustime += (end - start) * 1000 / CPU_MHz;
    }

    double avg_ustime = total_ustime / TRYTIMES;

    free(arr);
    free(index);

    return (avg_ustime - emvisit_time) / PAGENUMS;
    
}

