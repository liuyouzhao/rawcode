#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <utils/util.h>

void *rc_malloc(size_t size);
void rc_dump_cache();

static void test_mem_malloc();

void test_mem()
{
    test_mem_malloc();    
}

static void test_mem_malloc()
{
    int ml = 0;
    void *p = NULL;    
    int i = 0;

    ml = rc_heap_left();
    printf("heap left %d Bytes\n", ml);

    for(i = 0; i < 30; i ++)
        p = rc_malloc(200);

    for(i = 0; i < 25; i ++)
        p = rc_malloc(140);

    for(i = 0; i < 15; i ++)
        p = rc_malloc(32);

    for(i = 0; i < 4; i ++)
        p = rc_malloc(2200);


    rc_dump_cache();
    ml = rc_heap_left();
    printf("heap left %d Bytes\n", ml);
}


