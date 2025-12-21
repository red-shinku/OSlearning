#include "../include/LRU.h"
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

void destroyipages(Ipages* ipages)
{
    for(int i=0; i < ipages->size; ++i)
        free(ipages->pages[i]);

    free(ipages->pages);
}

void print_LRU(Stack* stack, Ipages* ipages)
{
    //print page not in mem
    printf("------------------------------------------------------------------------------");
    printf("\nthose pages not in memory:\n");
    Ipagenode** pages = ipages->pages;
    for(int i=0; i < ipages->size; ++i)
    {
        if(pages[i]->is_inmem == false)
            printf("%d ", pages[i]->no);
    }
    //print stack
    printf("\nshow the stack:\n");
    printf("|            |\n");    
    Stacknode* top = stack->top;
    Stacknode* snode = top->next;
    while (snode != top)
    {
        Ipagenode* pgnode = container_of(snode, Ipagenode, node);
        printf("|------------|\n");
        printf("| pageno: %d  |\n", pgnode->no);
        snode = snode->next;
    }
    printf("|____________|\n");
    printf("which page will u use next: ");
}