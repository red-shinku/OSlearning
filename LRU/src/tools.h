#ifndef TOOL_H
#define TOOL_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define container_of(ptr, type, member) ({ \
    const typeof(((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type, member));})

void die(const char *fmt, ...);

#endif //TOOL_H