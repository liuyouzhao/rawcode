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
    unsigned char *ptr_stack_heap_left;
    unsigned char *ptr_stack_heap_right;
    unsigned int size;
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
    rc_mm_slab_t *arr_slabs;
    
} rc_mm_cache_t;

static unsigned int s_slab_unit;
static unsigned int s_slab_nmax;
static unsigned int s_prsv_chunk;

/* heap & cache structure in data area */
static rc_mm_heap_t s_heap;
static rc_mm_cache_t s_cache;

void rc_memset(void* p, unsigned char v, unsigned int len);
void rc_memcpy(void* dst, void *src, unsigned int len);

static void _init_chunks(unsigned int chunk_size);
/**
global heap top
[top]                                         [low]
--------------------------------------------------
|management data|                    | user data |
--------------------------------------------------
^                                                ^
L                                                R
*/
int rc_mm_init()
{
    int i;

    s_heap.ptr_stack_heap_left = g_pt->heap_low;
    s_heap.ptr_stack_heap_right = g_pt->heap_top;
    s_heap.size = s_heap.ptr_stack_heap_right - s_heap.ptr_stack_heap_left;

    s_slab_unit = g_pt->slab_unit;
    s_slab_nmax = g_pt->slab_nmax;
    s_prsv_chunk = g_pt->prsv_chunk;

    /* clear heap mem */
    rc_memset(s_heap.ptr_stack_heap_left, 0, s_heap.size);
}

void rc_memset(void* p, unsigned char v, unsigned int len)
{
    for(; len > 0; len --) {
        *_PTRC_(_PTRC_(p) + len) = v;
    }
}

void rc_memcpy(void* dst, void *src, unsigned int len)
{
    for(; len > 0; len --) {
        *_PTRC_(_PTRC_(dst) + len) = *_PTRC_(_PTRC_(src) + len);
    }
}

void *rc_alloc(size_t size)
{
    return 0;
}

void rc_free()
{
}

static int _heap_fatal(unsigned int size)
{
    if((unsigned int)(s_heap.ptr_stack_heap_left + size) >=
        (unsigned int)(s_heap.ptr_stack_heap_right)) {
        return -1;
    }
    return 0;
}

static void _mov_left_heap_ptr(unsigned int size)
{
    if(_heap_fatal(size)) {
        __DEBUG_ERR__("[PMD] Memory deficiency");
        g_pt->panic();
    }
    s_heap.ptr_stack_heap_left += size;
}

static void _mov_right_heap_ptr(unsigned int size)
{
    if(_heap_fatal(size)) {
        __DEBUG_ERR__("[PMD] Memory deficiency");
        g_pt->panic();
    }
    s_heap.ptr_stack_heap_right -= size;
}

static void* _left_valid_next(unsigned int size)
{
    void *p = s_heap.ptr_stack_heap_left;
    _mov_left_heap_ptr(size);
    return p;
}

static void* _right_valid_next(unsigned int size)
{
    void *p = s_heap.ptr_stack_heap_right - size;
    _mov_right_heap_ptr(size);
    return p;
}

static void _init_slabs()
{
    int i;
    rc_mm_slab_t slab;

    /* Init cache structure, slabs array */
    for( i = 0; i < s_slab_nmax; i ++ ) {
        _init_chunks(i * s_slab_unit);
        //(s_cache.arr_slabs + i * sizeof(rc_mm_slab_t)) = 
    }
}

static void _init_chunks(unsigned int chunk_size)
{
    int i;
    rc_mm_chunk_t *prv = NULL;
    rc_mm_chunk_t *cur = NULL;
    rc_mm_chunk_t chunk;

    for( i = 0; i < s_prsv_chunk; i ++ ) {
        cur = _left_valid_next(sizeof(rc_mm_chunk_t));
        ((rc_mm_chunk_t*)cur)->data = _right_valid_next(chunk_size);

        if(prv != NULL) {
            prv->next = cur;
        }
        prv = cur;
    }
}
