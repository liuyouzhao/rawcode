#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <utils/util.h>

void *rc_malloc(size_t size);
void rc_memset(void* p, unsigned char v, unsigned int len);
void rc_dump_cache();

static void test_mem_malloc();

void test_mem()
{
    test_mem_malloc();    
}

static void test_mem_malloc()
{
    int ml = 0;
    char *p[4] = {0};
    int i = 0;
    int j = 0;

    ml = rc_heap_left();
    printf("heap left %d Bytes\n", ml);

#if 0
    for(i = 0; i < 30; i ++)
        p = rc_malloc(200);

    for(i = 0; i < 25; i ++)
        p = rc_malloc(140);

    for(i = 0; i < 15; i ++)
        p = rc_malloc(32);

    for(i = 0; i < 4; i ++)
        p = rc_malloc(2200);
#endif
    for(i = 0; i < 4; i ++) {
        p[i] = (char*)rc_malloc(600);
        rc_memset(p[i], 0, 600);
        for(j = 0; j < 600; j ++) {
            p[i][j] = 'a';
        }
    }
    rc_memset(p[2], '0', 600);
    //p[2][0] = 'b';
    rc_free(p[2]);
    printf("%s", p[2]);
    for(i = 0; i < 4; i ++) {
        printf("\n%s\n", p[i]);
    }
    

    rc_dump_cache();
    ml = rc_heap_left();
    printf("heap left %d Bytes\n", ml);
}


