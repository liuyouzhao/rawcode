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


ENTRY(_start)
SECTIONS
{
. = 0x10000;
startup : { startup.o(.text)}
.data : {*(.data)}
.bss : {*(.bss)}
. = ALIGN(8);
. = . + 0x4000; /* 4kB of stack memory */
stack_top = .;
. = . + 0x4000; /* 4kB of interrupt stack memory */
svc_stack_top = .;
. = . + 0x4000; /* 4kB of interrupt stack memory */
isr_stack_top = .;
. = . + 0x4000; /* little buffer of 4kB */
task_stack_top = .;
. = . + 0x800000;
heap_low = .;
. = . + 0x10000;
heap_top = .;
}

