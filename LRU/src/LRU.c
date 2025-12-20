#include "include/LRU.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

Ipages init_ipages(int pagenums)
{
    Ipagenode **pages = (Ipagenode**)malloc(pagenums * sizeof(*pages));
    for(int i=0; i < pagenums; ++i)
    {
        pages[i] = (Ipagenode*)malloc(sizeof(**pages));
        pages[i]->node.next = NULL;
        pages[i]->node.prev = NULL;
        pages[i]->no = i;
        pages[i]->is_inmem = false;
    }

    Ipages ipages;
    ipages.pages = pages;
    ipages.size = pagenums;
    return ipages;
}

void use_page(Stack* stack, Ipages *ipages, int need)
{
    //page fault -- illegal visit
    if(need >= ipages->size)
    {
        die("page fault -- illegal visit");
    }
    Ipagenode* needpage = ipages->pages[need];

    if(needpage->is_inmem)
        settop(stack, &needpage->node);
    else // page fault
    { 
        if(stack->size < stack->contain)
        {
            push(stack, &needpage->node);
            needpage->is_inmem = true;
        }
        else //do swap
            swap_page(stack, ipages, need);
    }
}

void swap_page(Stack* stack, Ipages *ipages, int need)
{
    Ipagenode *vpage = ipages->pages[need];
    Stacknode* pagenode = bottom(stack);
    if(pagenode == NULL)
    {
        die("swap fault, no page in mem");
    }
    Ipagenode *page = container_of(pagenode, Ipagenode, node);

    delnode(stack, &page->node);
    page->is_inmem = false;
    push(stack, &vpage->node);
    vpage->is_inmem = true;
}

void destroyipages(Ipages ipages)
{
    for(int i=0; i < ipages.size; ++i)
        free(ipages.pages[i]);

    free(ipages.pages);
}

void die(const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	vfprintf(stderr, fmt, argp);
	va_end(argp);
	fputc('\n', stderr);
	exit(1);
}