#ifndef PAGE_STACK_H
#define PAGE_STACK_H

#include "tools.h"

typedef struct Stacknode
{
    struct Stacknode *prev;
    struct Stacknode *next;
}Stacknode;

typedef struct Stack
{
    Stacknode *top;
    int size;
    int contain;
}Stack;

Stack* newstack(int size);
void push(Stack* stack, Stacknode* node);
void settop(Stack* stack, Stacknode* node);
void delnode(Stack* stack, Stacknode* node);
Stacknode* bottom(Stack* stack);
void delete_stack(Stack* stack);

#endif //PAGE_STACK_H