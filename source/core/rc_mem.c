#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <utils/util.h>
#include <port/port.h>

/*

cache:       slab_link&chunk_link:
|---------|  
| 0  slab0|->slab1->slab2
| 1       |    |
| 2       |    chunk0->chunk1->chunk2
| 3       |       ^
| ....... |  
| n       |  
-----------

global heap top
[top]                                         [low]
--------------------------------------------------
|management data|                    | user data |
--------------------------------------------------
^                                                ^
L                                                R

*/
typedef struct rc_mm_heap_s
{
    unsigned char *ptr_stack_heap_left;
    unsigned char *ptr_stack_heap_right;
    unsigned int size;
} rc_mm_heap_t;

typedef struct rc_mm_chunk_s
{
    unsigned char occupied;
    struct rc_mm_chunk_s *next;
    unsigned char *data;

} rc_mm_chunk_t;

typedef struct rc_mm_slab_s
{
    rc_mm_chunk_t *chunk_head;

    unsigned int alloced;

    struct rc_mm_slab_s *prev;
    struct rc_mm_slab_s *next;

} rc_mm_slab_t;

typedef struct rc_mm_cache_s
{
    rc_mm_slab_t *arr_slabs;
    
} rc_mm_cache_t;

/* 
 * s_slab_unit
 * slab unit is the unit-size of each slab, 
 * if 16B, the 1st 64B*1=64B,
 *             2nd 64B*2=128B,
 *             3rd 64B*3=192B,
 *             ....
*/
static unsigned int s_slab_unit;

/* 2^s_slab_para == s_slab_unit */
static unsigned int s_slab_para;

/* the max number of slabs */
static unsigned int s_slab_nmax;

/* chunks number to preserve */
static unsigned int s_prsv_chunk;

/* heap & cache structure in data area */
static rc_mm_heap_t s_heap;
static rc_mm_cache_t s_cache;

void rc_memset(void* p, unsigned char v, unsigned int len);
void rc_memcpy(void* dst, void *src, unsigned int len);

static void init_cache();
static void init_cache_slabs();
static rc_mm_slab_t * create_slab(int i);
static rc_mm_chunk_t* init_chunks(unsigned int chunk_size);

/**
 * check how much memory left
*/
unsigned int rc_heap_left();

int rc_mm_init()
{
    int i, j;

    s_heap.ptr_stack_heap_left = g_pt->heap_low;
    s_heap.ptr_stack_heap_right = g_pt->heap_top;
    s_heap.size = s_heap.ptr_stack_heap_right - s_heap.ptr_stack_heap_left;

    s_slab_unit = g_pt->slab_unit;
    s_slab_nmax = g_pt->slab_nmax;
    s_prsv_chunk = g_pt->prsv_chunk;

    /* calculate s_slab_para by s_slab_unit */
    i = j = 1;
    while((i = i * 2) != s_slab_unit) {
        if(i > s_slab_unit) {
            __DEBUG_ERR__("Poring error, slag unit must be 2^n");
            g_pt->panic();
        }
        j ++;
    }
    s_slab_para = j;

    /* clear heap mem */
    rc_memset(s_heap.ptr_stack_heap_left, 0, s_heap.size);

    init_cache();
    i = rc_heap_left();
    printf("heap left %d Bytes\n", i);
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

void *rc_malloc(size_t size)
{
    int i;
    rc_mm_slab_t *p_slab;
    rc_mm_chunk_t *p_chunk;
    int index = size >> s_slab_para;

    g_pt->enter_critical();
    
    p_slab = &(s_cache.arr_slabs[index]);

    while(p_slab) {

        if(p_slab->alloced < s_prsv_chunk) {
            p_chunk = p_slab->chunk_head;
            while(p_chunk->occupied) p_chunk = p_chunk->next;
            return p_chunk->data;
        }

        if(p_slab->next == NULL) {
            p_slab->next = create_slab(index);
        }
        p_slab = p_slab->next;
    }

    g_pt->exit_critical();
    return 0;
}

void *rc_calloc(size_t size)
{
    return 0;
}

void *rc_realloc(size_t size)
{
    return 0;
}

void rc_free()
{
}

unsigned int rc_heap_left()
{
    return s_heap.ptr_stack_heap_right - s_heap.ptr_stack_heap_left;
}

static int heap_fatal(unsigned int size)
{
    if((unsigned int)(s_heap.ptr_stack_heap_left + size) >=
        (unsigned int)(s_heap.ptr_stack_heap_right)) {
        return -1;
    }
    return 0;
}

static void mov_left_heap_ptr(unsigned int size)
{
    if(heap_fatal(size)) {
        __DEBUG_ERR__("[PMD] Memory deficiency");
        g_pt->panic();
    }
    s_heap.ptr_stack_heap_left += size;
}

static void mov_right_heap_ptr(unsigned int size)
{
    if(heap_fatal(size)) {
        __DEBUG_ERR__("[PMD] Memory deficiency");
        g_pt->panic();
    }
    s_heap.ptr_stack_heap_right -= size;
}

static void* frm_left_valid_next(unsigned int size)
{
    void *p = s_heap.ptr_stack_heap_left;
    mov_left_heap_ptr(size);
    return p;
}

static void* frm_right_valid_next(unsigned int size)
{
    void *p = s_heap.ptr_stack_heap_right - size;
    mov_right_heap_ptr(size);
    return p;
}

/**
 * init cache for all kinds of slabs

cache:        slab_link&chunk_link:
size is n * s_slab_unit
|---------|  |
| 0  slab0|  | ->slab1->slab2
| 1       |  |     |
| 2       |  |     chunk0->chunk1->chunk2
| 3       |  |       ^
| ....... |  |
| n       |  |

*/
static void init_cache()
{
    s_cache.arr_slabs = (rc_mm_slab_t*)frm_left_valid_next(sizeof(rc_mm_slab_t) * s_slab_nmax);
    __DEBUG__
    init_cache_slabs();
    __DEBUG__
}

static void init_cache_slabs()
{
    int i;
    rc_mm_slab_t *p;

    /* Init cache structure, slabs array */
    for( i = 0; i < s_slab_nmax; i ++ ) {
        p = &(s_cache.arr_slabs[i]);
        p->prev = p->next = 0;
        p->alloced = 0;
        p->chunk_head = init_chunks(i * s_slab_unit);
        __DEBUG__
    }
}

static rc_mm_slab_t * create_slab(int i)
{
    rc_mm_slab_t *p = (rc_mm_slab_t*)frm_left_valid_next(sizeof(rc_mm_slab_t));
    p->prev = p->next = 0;
    p->alloced = 0;
    p->chunk_head = init_chunks(i * s_slab_unit);
    return p;
}

/**
Init all slab0s from cache table

slab0->NULL
  |
  chunk0->chunk1->chunk2
     ^
*/
static rc_mm_chunk_t* init_chunks(unsigned int chunk_size)
{
    int i;
    rc_mm_chunk_t *prv = NULL;
    rc_mm_chunk_t *head = NULL;
    rc_mm_chunk_t *cur = NULL;

    prv = cur = head = frm_left_valid_next(sizeof(rc_mm_chunk_t));
    ((rc_mm_chunk_t*)cur)->data = NULL;
    ((rc_mm_chunk_t*)cur)->occupied = 0;

    for( i = 0; i < s_prsv_chunk - 1; i ++ ) {

        cur->next = frm_left_valid_next(sizeof(rc_mm_chunk_t));
        cur = cur->next;

        ((rc_mm_chunk_t*)cur)->data = NULL;
        ((rc_mm_chunk_t*)cur)->occupied = 0;

        if(prv != NULL) {
            prv->next = cur;
        }
        prv = cur;
    }

    return head;
}
