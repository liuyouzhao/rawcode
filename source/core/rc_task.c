/********************************************************************************
 * core/rc_task.c
 *
 *   Copyright (C) 2016,2017 Steven Liu. All rights reserved.
 *   Author: Steven Liu <liuyouzhao@hotmail.com>
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

#include <string.h>
#include <utils/util.h>
#include <utils/list.h>
#include <port/port.h>
#include <arch.h>

/**
 rc_task_t
 One rc_task_t pointer for each task user created.
 @regs: rc_registers_t given in arch.c
 @prio: priority
 @stack_low: the low address of this task's stack
 @stack_size: stack_low + stack_size == stack_top
 @task_name: whatever
 @entry: entry function pointer
 @para: fixed pointer as parameter be passed into the entry function
*/
typedef struct rc_task_s
{
    rc_registers_t regs;

    unsigned int prio;

    arch_stack_type stack_low;
    unsigned int stack_size;

    char task_name[KC_TASK_MAX_NAME_LEN];

    void (*entry)(void* p);
    void *para;

    __L_NODE__
} rc_task_t;

/* All tasks list */
static list_t s_lst_tasks;

/* Ready list */
static list_t s_lst_ready;

/* Runing task */
static rc_task_t *s_ptsk_running = 0;

/* stack address the next new thread begin from as its stack_low */
static unsigned char *s_cur_stack_ptr;

/* entering section counter */
static int s_entered;

/* last tick number */
static unsigned long s_last_tick = 0;

#if (!CONFIG_ABTASK)
static rc_task_t *s_ptsk1 = 0;
static rc_task_t *s_ptsk2 = 0;
#endif

/**
 * inner static functions
*/
static void put_to_ready_list(rc_task_t *tsk);
static void task_tick_switch();
static void rc_task_sys_tick();
static void rc_task_swi();

/**
 * mem functions
 */
void *rc_kmalloc(size_t size);

/*
 * Task API
*/

/**
 * Initialize task-manager foundation
 * Don't call this funtion as user
*/
int rc_task_init()
{
    s_cur_stack_ptr = g_pt->stack_low;

    s_lst_tasks.len = 0;
    s_lst_ready.len = 0;

    s_entered = 0;

    g_pt->arch_bind_systick(rc_task_sys_tick);
    g_pt->arch_bind_swi(rc_task_swi);
}

/**
 * Create new task
 * your task function will never end, otherwise it will cause problems
 * like for(;;) or while(1) available, for keeping coherent jumping logic.
*/
int rc_task_create(const char* name, void (*pfunc) (void* para), 
                            unsigned int stacksize, unsigned int prior, void *para)
{
    rc_task_t *tsk;

    g_pt->enter_critical();

    if(s_lst_tasks.len >= KC_TASK_MAX_NUM || 
        s_cur_stack_ptr - stacksize < g_pt->stack_top) {
        __DEBUG__
        return -1;
    }

    tsk = (rc_task_t*) rc_kmalloc(sizeof(rc_task_t));

    if(tsk == NULL) {
        __DEBUG_ERR__("memory is not enough");
        return -1;
    }

    memcpy(tsk->task_name, name, strlen(name));
    tsk->prio = prior;
    tsk->para = para;
    tsk->stack_low = (unsigned int)s_cur_stack_ptr;
    tsk->stack_size = stacksize;
    tsk->entry = pfunc;

    kprintf("tsk->entry: %p\n", tsk->entry);
    g_pt->task_registers_init(&(tsk->regs), tsk->entry, tsk->para, tsk->stack_low);
    put_to_ready_list(tsk);
    s_cur_stack_ptr -= stacksize;
    g_pt->exit_critical();
    return 0;
}

/**
 * Enter section means pause switching temporarily
*/
void rc_task_enter_section()
{
    s_entered ++;
    s_last_tick = g_pt->global_tick;
}

/**
 * Resume the switching 
*/
void rc_task_exit_section()
{
    s_entered --;
    if(s_entered < 0) s_entered = 0;
}

/**
 * Resume switching immediately
*/
void rc_task_clear_section()
{
    s_entered = 0;
}

/**
 * Trigger SWI manually
*/
void rc_task_interrupt()
{
    g_pt->enter_critical();
    rc_task_clear_section();
    g_pt->task_interrupt();
}

/*
 * get running task
*/
void *rc_task_running()
{
    return s_ptsk_running;
}

/**
 * Move current runing task into blocked list
                     | Blocked List |
                     |     tsk1     |
                     |     tsk2     |
  s_ptsk_running --> |     ...      |

                     | Ready List   |
  s_ptsk_running <-- |     tsk3     |
                     |     tsk4     |
                     |     ...      |
*/
void rc_task_suspend()
{
    l_node_t *pn = 0;

    g_pt->enter_critical();

    if(s_lst_ready.len == 0) {
        __DEBUG_ERR__("No alive task left, sys will block forever");
        s_ptsk_running = NULL;
        goto exit;
    }

    pn = list_pop_head(&s_lst_ready);
    s_ptsk_running = list_node_container(rc_task_t, *pn);
exit:
    g_pt->exit_critical();
}

void rc_task_ready(void *tsk)
{
    put_to_ready_list((rc_task_t*)tsk);
}

/**
 * Switch running task
                     | Ready List |
  s_ptsk_running <-- |     tsk1     |
                     |     tsk2     |
                     |     ...      |
                     |              |
                     |     tsk8     |
                     |     tsk9     |
  s_ptsk_running --> |     ...      |
*/
void rc_task_switch()
{
#if (!CONFIG_ABTASK)
    l_node_t *pn;
    rc_task_t *last;

    if(s_lst_ready.len == 0) {
        if(s_ptsk_running != NULL) {
             g_pt->task_switch(&(s_ptsk_running->regs), &(s_ptsk_running->regs),
                            s_ptsk_running->stack_low, s_ptsk_running->stack_size,
                            s_ptsk_running->para);
        }
        goto exit;
    }

    if(s_ptsk_running == NULL) {
        pn = list_pop_head(&s_lst_ready);
        s_ptsk_running = list_node_container(rc_task_t, *pn);
        g_pt->task_switch(&(s_ptsk_running->regs), NULL,
                            s_ptsk_running->stack_low, s_ptsk_running->stack_size,
                            s_ptsk_running->para);
        goto exit;
    }
    last = s_ptsk_running;
    put_to_ready_list(s_ptsk_running);

    pn = list_pop_head(&s_lst_ready);
    s_ptsk_running = list_node_container(rc_task_t, *pn);

    g_pt->task_switch(&(s_ptsk_running->regs), &(last->regs),
                        s_ptsk_running->stack_low, s_ptsk_running->stack_size,
                        s_ptsk_running->para);
exit:
    return;
#else
    rc_task_t *last;
    if(s_ptsk_running == NULL) {
        s_ptsk_running = s_ptsk1;
        g_pt->task_switch(&(s_ptsk_running->regs), NULL,
                            s_ptsk_running->stack_low, s_ptsk_running->stack_size,
                            s_ptsk_running->para);
        return;
    }
    last = s_ptsk_running;
    s_ptsk_running = s_ptsk_running == s_ptsk1 ? s_ptsk2 : s_ptsk1;
    g_pt->task_switch(&(s_ptsk_running->regs), &(last->regs),
                    s_ptsk_running->stack_low, s_ptsk_running->stack_size,
                    s_ptsk_running->para);
#endif
}

static void rc_task_sys_tick()
{
    kprintf("tick->%d\n", g_pt->global_tick);
    if(s_entered > 0) {
        return;
    }
    task_tick_switch();
}

static void rc_task_swi()
{
    if(s_entered) {
        return;
    }
    task_tick_switch();
}

static void task_tick_switch()
{
    rc_task_switch();
}

static void put_to_ready_list(rc_task_t *tsk)
{
#if (!CONFIG_ABTASK)
    l_node_t *p = s_lst_ready.p_head;
    rc_task_t *cur_tsk = NULL;
    while(p)
    {
        cur_tsk = list_node_container(rc_task_t, *p);
        if(cur_tsk->prio < tsk->prio) {
            break;
        }
        p = p->p_next;
    }
    if(cur_tsk == NULL) {
        list_add_tail(&s_lst_ready, tsk);
    }
    else {
        list_insert_ptr_next(&s_lst_ready, cur_tsk, tsk);

        p = s_lst_ready.p_head;
        while(p) {
            p = p->p_next;
        }

    }
#else
    if(s_ptsk1 == 0)
        s_ptsk1 = tsk;
    else if(s_ptsk2 == 0)
        s_ptsk2 = tsk;
#endif
}

