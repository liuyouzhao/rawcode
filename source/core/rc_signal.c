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
#include <utils/list.h>
#include <utils/debug.h>
#include <port/port.h>
#include <arch.h>

typedef struct rc_signal_lstnr_s
{
    void *tsk;
    __L_NODE__
} rc_signal_lstnr_t;

typedef struct rc_signal_bndle_s
{
    list_t lstnrs;
    int idle;
} rc_signal_bndle_t;

typedef unsigned int rc_signal_t;

static unsigned int s_idle = 0;
static rc_signal_bndle_t *s_bundles[KC_SIGNAL_MAX];


/* rc_task.c functions */
void *rc_task_running();
void rc_task_suspend();
void rc_task_switch();

/**
 * mem functions
 */
void *rc_kmalloc(size_t size);

list_t rc_signal_init()
{
    int i = 0;
    g_pt->enter_critical();

    for( ; i < KC_SIGNAL_MAX; i ++ ) {
        s_bundles[i] = (rc_signal_bndle_t*) rc_kmalloc(sizeof(rc_signal_bndle_t));
        s_bundles[i]->idle = 1;
    }
    s_idle = 0;

    g_pt->exit_critical();
}

void rc_signal_create(rc_signal_t *signal)
{
    g_pt->enter_critical();

    if(s_idle >= KC_SIGNAL_MAX) {
        __KDEBUG_ERR__("signal number overflow");
        goto exit;
    }
    *signal = s_idle;

    while(!s_bundles[s_idle]->idle && s_idle < KC_SIGNAL_MAX) {
        s_idle ++;
    }
exit:
    g_pt->exit_critical();
    return;
}

int rc_signal_recv(rc_signal_t signal, unsigned int timeout)
{
    rc_signal_lstnr_t *lstnr = NULL;

    g_pt->enter_critical();

    lstnr = (rc_signal_lstnr_t*) rc_kmalloc(sizeof(rc_signal_lstnr_t));
    lstnr->tsk = (void*) rc_task_running();

    if(s_bundles[signal]->idle) {
        s_bundles[signal]->idle = 0;
    }

    list_add_tail(&s_bundles[signal]->lstnrs, lstnr);

    rc_task_suspend();
    rc_task_switch();
}

int rc_signal_post()
{
}
