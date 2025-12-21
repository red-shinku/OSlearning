#ifndef LRU_H
#define LRU_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../src/stack.h"
#include "../src/tools.h"

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
void destroyipages(Ipages* ipages);
void print_LRU(Stack*, Ipages*);

#endif //LRU_H