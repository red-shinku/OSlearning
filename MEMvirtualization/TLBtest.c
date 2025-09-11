#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdatomic.h>
#include <x86intrin.h>
#include <stdarg.h>

//disrupt array
static void shuffle(int *array, int size) {
    srand((unsigned int)time(NULL));

    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);         
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

//linux : get your system's page size
long get_page_size(){
    return sysconf(_SC_PAGESIZE);
}

double tlb(long USE_MEM, int PAGENUMS, long PAGE_SIZE, long TRYTIMES, float CPU_MHz){

    //make sure work on the same cpu
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    sched_setaffinity(0, sizeof(cpuset), &cpuset);

    int jump = PAGE_SIZE/sizeof(int);
    int *index = NULL;
    int *arr = NULL;

    //init index[] and arr[]
    if(USE_MEM > PAGENUMS * PAGE_SIZE){
        index = (int*)malloc(USE_MEM / PAGE_SIZE * sizeof(int));
        arr = (int*)malloc(USE_MEM);

        if(arr == NULL || index == NULL){
            perror("failed to malloc array");
            return -1;
        }

        for(int i=0; i < USE_MEM / sizeof(int); i += jump){
            arr[i] = i;
        }
            //use index[] to visit arr randomly
        index[0] = 0;
        for(int i=1; i < USE_MEM / PAGE_SIZE; ++i){
            index[i] = index[i-1] + jump;
        }

        shuffle(index, USE_MEM / PAGE_SIZE);

    }else{
        index = (int*)malloc(PAGENUMS * sizeof(int));
        arr = (int*)malloc(PAGENUMS * PAGE_SIZE);

        if(arr == NULL || index == NULL){
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
    }


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

