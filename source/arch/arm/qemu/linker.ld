/*
 * Copyright (c) 2004-2008 Texas Instruments
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */



/**
 * |-------------------------------------------------|
 * [text+data+bss]0x10000---0x14000 [16KB]
 * [stack]        0x14000---0x15000 [4KB]
 * [svc_stack]    0x15000---0x16000 [4KB] (supervisor calls)
 * [isr_stack]    0x16000---0x17000 [4KB] (interrupt status register)
 * [task_stack]   0x17000---0x1f000 [32KB]
 * [heap_low]     0x1f000---0x2f000 [64KB](heap begin)
 * [heap]         0x2f000-- 
*/
ENTRY(_start)
SECTIONS
{
. = 0x10000;
/* the startup.o in out/build */
startup : { ../build/out/startup.o(.text)}
.data : {*(.data)}
.bss : {*(.bss)}
. = ALIGN(8);
. = . + 0x4000; /* 4kB of stack memory */
stack_top = .;  /* 0x14000 */
. = . + 0x1000; /* 4kB of interrupt stack memory */
svc_stack_top = .; /* 0x15000 */
. = . + 0x1000; /* 4kB of interrupt stack memory */
isr_stack_top = .; /* 0x16000 */
. = . + 0x1000; /* little buffer of 4kB */
task_stack_top = .; /* 0x17000 */
. = . + 0x8000; /* 32KB task stack */
heap_low = .;   /* 0x1f000 */
. = . + 0x10000;/* 64KB heap */
heap_top = .;   /* 0x2f000 */
}

