/********************************************************************************
 * utils/list.h
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
#ifndef __INCLUDE_DEBUG_H
#define __INCLUDE_DEBUG_H

#include <stdarg.h>

/*****************************
 * uart0 printf
********************************/
extern unsigned char* glb_output_uart_addr;
void dbg_printf_ext(const char* text, ...);
void dbg_printf_ext_kernel(const char* text, ...);
void dbg_printf(const char* text);
void dbg_dump_stack();

#define printf dbg_printf_ext
#define kprintf dbg_printf_ext_kernel
#define __DEBUG__ printf("%s [%s:%d]\n", __FILE__, __FUNCTION__, __LINE__);
#define __KDEBUG__ kprintf("%s [%s:%d]\n", __FILE__, __FUNCTION__, __LINE__);

#define __DEBUG_ERR__(a) printf("[Fatal error] "a"\n");
#define __KDEBUG_ERR__(a) kprintf("[Fatal error] "a"\n");
#endif
