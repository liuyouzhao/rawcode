/********************************************************************************
 * utils/list.h
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
#ifndef __INCLUDE_PORT_H
#define __INCLUDE_PORT_H

typedef struct port_s
{
    /* Basic global functions */
    void (*reset)();
    void (*panic)();

    /* Enter&exit critical functions */
    void (*enter_critical)();
    void (*exit_critical)();

    /* task functions */
    void (*task_registers_init)(void *regs, void *entry, void *para, unsigned int stack_low);
    void (*task_switch)(void *regs, void *last_regs, 
                        unsigned int stack_low, unsigned int stack_size,
                        void *para);
    void (*task_interrupt)();

    /* interrupt setup functions */
    void (*arch_bind_systick)(void (*systick)());
    void (*arch_bind_swi)(void (*swi)());


    unsigned long   critical_nesting;

    unsigned char*  uart0_addr;
    unsigned char*  uart1_addr;

    unsigned char*  stack_top;
    unsigned char*  stack_low;
    unsigned char*  heap_low;
    unsigned char*  heap_top;
    unsigned int    slab_unit;
    unsigned int    slab_nmax;
    unsigned int    prsv_chunk;

    unsigned long   global_tick;

} port_t;

port_t *g_pt;

#endif /* __INCLUDE_PORT_H */
