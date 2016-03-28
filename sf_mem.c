/* Written by jic 2013-03 */
#include "sf_include.h"

void *sf_malloc(size_t size)
{
    return calloc(1, size);
}

void *sf_calloc(size_t size)
{
    return calloc(1, size);
}

char * sf_strdup(char* str)
{
    return strdup(str);
}

void sf_free(void *ptr)
{
    if (ptr)
        return free(ptr);
}

