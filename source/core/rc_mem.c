/********************************************************************************
 * core/rc_mem.c
 *
 *   Copyright (C) 2016,2017 Frodo Liu. All rights reserved.
 *   Author: Frodo Liu <liuyouzhao@hotmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name RawCode nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <utils/util.h>
#include <port/port.h>

/**
This file is for memory management implementation.
The main idea is based on Slag-Thunk algorithm.

The following comments show how the structures like to buildup
the mm logic.

cache:       slab_link&chunk_link:
|---------|  
| 0  slab0|->slab1->slab2
| 1       |    |
| 2       |    chunk0->chunk1->chunk2
| 3       |       ^
| ....... |  
| n       |  
-----------
*/

/**
rc_mm_heap_t
In global heap, here descripts memory space from top to low.
ptr_stack_heap_left begins from left side as ptr_stack_heap_right from right.

[heap top]                              [heap low]
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
    unsigned int id;
    int occupied;
    struct rc_mm_chunk_s *next;
    struct rc_mm_chunk_s *prev;
    unsigned char *data;

} rc_mm_chunk_t;

typedef struct rc_mm_slab_s
{
    unsigned int id;
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
static void * frm_right_valid_next(unsigned int size);
static void * frm_left_valid_next(unsigned int size);
static void dump_cache();
static void *_malloc(size_t size);
static void _free(void *p);
void *rc_malloc(size_t size);

/**
 * check how much memory left
*/
unsigned int rc_heap_left();

int rc_mm_init()
{
    int i, j;

    g_pt->enter_critical();
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

    g_pt->exit_critical();
}

void rc_memset(void* p, unsigned char v, unsigned int len)
{
    int i;
    for(i = 0; i < len; i ++) {
        *_PTRC_(_PTRC_(p) + i) = v;
    }
}

void rc_memcpy(void* dst, void *src, unsigned int len)
{
    int i;
    for(i = 0; i < len; i ++) {
        *_PTRC_(_PTRC_(dst) + i) = *_PTRC_(_PTRC_(src) + i);
    }
}

void *rc_malloc(size_t size)
{
    void *ptr = 0;
    g_pt->enter_critical();
    ptr = _malloc(size);
    g_pt->exit_critical();
    return ptr;
}

void *rc_kmalloc(size_t size)
{
    void *ptr = 0;
    ptr = _malloc(size);
    return ptr;
}

void *rc_calloc(size_t size)
{
    void *p = 0;
    p = rc_malloc(size);
    rc_memset(p, 0, size);
    return p;
}

void rc_free(void *p)
{
    g_pt->enter_critical();
    _free(p);
    g_pt->exit_critical();
}

void rc_kfree(void *p)
{
    _free(p);
}

/**
 * return how many heap left
*/
unsigned int rc_heap_left()
{
    return s_heap.ptr_stack_heap_right - s_heap.ptr_stack_heap_left;
}

void rc_dump_cache()
{
    int i, j;
    void *pp;
    rc_mm_slab_t *ps;
    rc_mm_chunk_t *pc;

    g_pt->enter_critical();

    for( i = 0; i < s_slab_nmax; i ++ ) {
        printf("[%d BYTES SLABS]", i * s_slab_unit);

        ps = &(s_cache.arr_slabs[i]);
        if(ps == NULL) {
            __DEBUG_ERR__("Error, here slab is NULL");
        }

        printf("\n");
        while(ps) {
            printf("-----slab-%d [%p]--------\n", ps->id, ps);

            pc = ps->chunk_head;
            while(pc) {
                if(pc->data != NULL)
                    pp = *(rc_mm_chunk_t**)*(int*)(pc->data - sizeof(unsigned int));
                else
                    pp = NULL;
                printf("-----------chunk(%p:[%d] %d %x %p) \n",
                        pc, pc->id, pc->occupied, pc->data, pp);
                pc = pc->next;
            }
            printf("\n");
            ps = ps->next;
        }
    }
    g_pt->exit_critical();
}

static void *_malloc(size_t size)
{
    int i;
    rc_mm_slab_t *p_slab;
    rc_mm_chunk_t *p_chunk;
    unsigned int *chunk_head_addr;
    int index = size >> s_slab_para; /* size / s_slab_unit */

    while(index * s_slab_unit < size) {
        index ++;
    }

    p_slab = &(s_cache.arr_slabs[index]);

    while(p_slab) {

        if(p_slab->alloced < s_prsv_chunk) {
            p_chunk = p_slab->chunk_head;
            while(p_chunk->occupied) p_chunk = p_chunk->next;

            if(p_chunk->data != NULL) {
                p_slab->alloced ++;
                p_chunk->occupied = 1;
                return p_chunk->data;
            }

            p_chunk->data = frm_right_valid_next(index * s_slab_unit);
            chunk_head_addr = frm_right_valid_next(sizeof(unsigned int));
            *chunk_head_addr = (unsigned int)&(p_slab->chunk_head);

            p_chunk->occupied = 1;
            p_slab->alloced ++;

            return p_chunk->data;
        }

        if(p_slab->next == NULL) {
            p_slab->next = create_slab(index);
            p_slab->next->prev = p_slab;
            p_slab->next->id = p_slab->id + 1;
        }
        p_slab = p_slab->next;
    }
    return 0;
}

static void _free(void *p)
{
    unsigned int *chunk_head_addr = (unsigned int*)(p - sizeof(unsigned int));
    rc_mm_chunk_t **pp_chunk = (rc_mm_chunk_t**)(*chunk_head_addr);
    rc_mm_chunk_t *chunk = *pp_chunk;
    rc_mm_slab_t *slab = _SELF_(rc_mm_slab_t, chunk_head, (*pp_chunk));

    while(chunk->data != p) {
        chunk = chunk->next;
        if(chunk == NULL) {
            __DEBUG_ERR__("free wrong address");
            kprintf("%p\n", p);
            g_pt->panic();
        }
    }
    chunk->occupied = 0;
    slab->alloced --;
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
    s_heap.ptr_stack_heap_left += size;
}

static void mov_right_heap_ptr(unsigned int size)
{
    s_heap.ptr_stack_heap_right -= size;
}

static void * frm_left_valid_next(unsigned int size)
{
    void *p;
    if(heap_fatal(size)) {
        __DEBUG_ERR__("fatal mem size");
        return NULL;
    }
    p = s_heap.ptr_stack_heap_left;
    mov_left_heap_ptr(size);
    return p;
}

static void * frm_right_valid_next(unsigned int size)
{
    void *p;
    if(heap_fatal(size)) {
        __DEBUG_ERR__("fatal mem size");
        return NULL;
    }
    p = s_heap.ptr_stack_heap_right - size;
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
    init_cache_slabs();
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
        p->id = i;
        p->chunk_head = init_chunks(i * s_slab_unit);
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
    ((rc_mm_chunk_t*)cur)->id = 0;
    ((rc_mm_chunk_t*)cur)->data = NULL;
    ((rc_mm_chunk_t*)cur)->occupied = 0;
    ((rc_mm_chunk_t*)cur)->prev = NULL;

    for( i = 0; i < s_prsv_chunk - 1; i ++ ) {

        cur->next = frm_left_valid_next(sizeof(rc_mm_chunk_t));
        cur->next->prev = cur;
        cur = cur->next;
        ((rc_mm_chunk_t*)cur)->id = i + 1;
        ((rc_mm_chunk_t*)cur)->data = NULL;
        ((rc_mm_chunk_t*)cur)->occupied = 0;

        if(prv != NULL) {
            prv->next = cur;
        }
        prv = cur;
    }

    return head;
}
