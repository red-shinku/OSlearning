#include "include/LRU.h"
#include <stdio.h>
#define MEM_PAGE_NUMS 10
#define MAX_MEM 6

int choose()
{
    int chooseno;
    scanf("%d", &chooseno);
    return chooseno;
}

int main()
{   
    Ipages ipages = init_ipages(MEM_PAGE_NUMS);
    Stack* stack = newstack(MAX_MEM);
    while (1)
    {
        print_LRU(stack, &ipages);
        use_page(stack, &ipages, choose());
    }
}