#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <utils/util.h>
#include <port/port.h>

/*
|---------|
| 0 [16B] |->slab0->slab1->slab2
| 1       |    |
| 2       |    chunk0->chunk1->chunk2
| 3       |      ^
| ....... |
| n[n*16B]|
*/

typedef struct rc_mm_heap_s
{
    unsigned char *ptr_stack_heap_low;
    unsigned char *ptr_stack_heap_top;
} rc_mm_heap_t;

typedef struct rc_mm_chunk_s
{
    struct rc_mm_chunk_s *next;

    unsigned char *data;

} rc_mm_chunk_t;

typedef struct rc_mm_slab_s
{
    rc_mm_chunk_t *chunks;

    unsigned int alloced;

    struct rc_mm_slab_s *prev;
    struct rc_mm_slab_s *next;

} rc_mm_slab_t;

typedef struct rc_mm_cache_s
{
    rc_mm_heap_t cache;
    rc_mm_slab_t *arr_slabs;
    
} rc_mm_cache_t;

static rc_mm_cache_t *s_cache;

/**
global heap top
[top]                                         [low]
--------------------------------------------------
|2kb cache|
--------------------------------------------------

*/
int rc_mm_init()
{
}

void *rc_alloc(size_t size)
{
    return 0;
}


void rc_free()
{
}

static void _mov_heap_top(rc_mm_heap_t *heap, unsigned int size)
{
    if(heap->ptr_stack_heap_top - size <= heap->ptr_stack_heap_low) {
        __DEBUG_ERR__("[PMD] Memory deficiency");
        g_pt->panic();
    }
    heap->ptr_stack_heap_top -= size;
}

static void _init_slabs()
{}
