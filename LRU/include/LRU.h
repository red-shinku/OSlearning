#ifndef LRU_H
#define LRU_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "src/stack.h"

#define MEM_PAGE_NUMS 10
#define container_of(ptr, type, member) ({ \
    const typeof(((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type, member));})

typedef struct Ipagenode
{
    Stacknode node;
    int no;
    bool is_inmem;
}Ipagenode;

typedef struct Ipages
{
    Ipagenode **pages;
    int size;
}Ipages;

Ipages init_ipages(int pagenums);
void use_page(Stack*, Ipages*, int need);
static void swap_page(Stack*, Ipages* ipages, int need);
void destroyipages(Ipages ipages);

static void die(const char *fmt, ...);

#endif //LRU_H