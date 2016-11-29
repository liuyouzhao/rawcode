#include <versatile.h>
#define CONFIG_SYS_TEXT_BASE		0x10000


.globl _start
_start:
/*
    ldr pc, _reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort //12
	ldr	pc, _data_abort  //16
	ldr	pc, _not_used  //20
	ldr	pc, _irq  //24
	ldr	pc, _fiq  //28
*/
	ldr sp, = .
	b	reset
	b .
/*
_zero:
    .word 0
_reset:
    .word reset
_undefined_instruction:
	.word undefined_instruction
_software_interrupt:
	.word software_interrupt
_prefetch_abort:
	.word prefetch_abort
_data_abort:
	.word data_abort
_not_used:
	.word not_used
_irq:
	.word irq
_fiq:
	.word fiq

	.balignl 16,0xdeadbeef
*/

_vectors_end:

reset:
	mrs	r0,cpsr
	bic	r0,r0,#0x1f
	orr	r0,r0,#0xd3
	msr	cpsr,r0
    bl steven


.globl _TEXT_BASE
_TEXT_BASE:
	.word	0x10000


.globl _bss_start_ofs
_bss_start_ofs:
	.word __bss_start - _start

.globl _bss_end_ofs
_bss_end_ofs:
	.word __bss_end__ - _start

.globl _end_ofs
_end_ofs:
	.word _end - _start


/*
.global _Start
_Start:
LDR sp, = sp_top
BL steven
B .
*/
