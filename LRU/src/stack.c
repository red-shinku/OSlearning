#include "stack.h"
#include <stdlib.h>

Stack* newstack(int contain)
{
    Stacknode* top = (Stacknode*)malloc(sizeof(*top));
    top->prev = top;
    top->next = top;
    Stack* stack = (Stack*)malloc(sizeof(*stack));
    stack->top = top;
    stack->contain = contain;
    return stack;
}

void push(Stack* stack, Stacknode* node)
{
    if(stack->size == stack->contain)
        return;
    if(node->next != NULL || node->prev != NULL)
        return;
    Stacknode* top = stack->top;
    node->next = top->next;
    top->next = node;
    node->prev = top;
    node->next->prev = node;
    ++stack->size;
}

void settop(Stack* stack, Stacknode* node)
{
    if(node->next == NULL || node->prev == NULL)
    {
        die("settop failed. when try to move a node doesnt in stack");
    }
    delnode(stack, node);
    push(stack, node);
}

void delnode(Stack* stack, Stacknode* node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = NULL;
    node->prev = NULL;
    --stack->size;
}

Stacknode* bottom(Stack* stack)
{
    Stacknode *btm = stack->top->prev;
    if(btm == stack->top)
        return NULL;
    else
        return btm;
}

void delete_stack(Stack* stack)
{
    free(stack->top);
    free(stack);
}