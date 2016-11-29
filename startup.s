#include <versatile.h>
#define CONFIG_SYS_TEXT_BASE		0x10000

.globl _start
_start:
    ldr pc, _reset //00
    ldr	pc, _undefined_instruction // 04
    ldr	pc, _software_interrupt //08
    ldr	pc, _prefetch_abort //0c
    ldr	pc, _data_abort  //10
    ldr	pc, _not_used  //14
    ldr	pc, _irq  //18
    ldr	pc, _fiq  //1c
    _reset:
        .word __reset
    _undefined_instruction:
	    .word __undefined_instruction
    _software_interrupt:
	    .word __software_interrupt
    _prefetch_abort:
	    .word __prefetch_abort
    _data_abort:
	    .word __data_abort
    _not_used:
	    .word __not_used
    _irq:
	    .word __irq
    _fiq:
	    .word __fiq
    .balignl 16,0xdeadbeef
	
	
__reset:
    MRS r1, cpsr
    BIC r1, r1, #0x40
    MSR cpsr_c, r1
    mov sp,#0x20000
    bl steven
    b .
    
    
.globl _test
_test:
    ldr pc, _sec
    _sec: .word 0x04
    
.globl do_swi
do_swi:
    push {lr}
    swi 0x123
    pop {lr}
    bx lr

    
.globl _insert_code
_insert_code:
    str r0, [r1]
    bx lr
    
    
.globl _query_code
_query_code:
    ldr r0, [r0]
    bx lr
    

__undefined_instruction:
    ldr	pc, undefined_instruction_handle
    bx lr
    undefined_instruction_handle:.word undefined_instruction
    
__software_interrupt:
    ldr	pc, software_interrupt_handle
    bx lr
    software_interrupt_handle:.word software_interrupt
    
__prefetch_abort:
    ldr	pc, prefetch_abort_handle
    bx lr
    prefetch_abort_handle:.word prefetch_abort
    
__data_abort:
    ldr	pc, data_abort_handle
    bx lr
    data_abort_handle:.word data_abort
    
__not_used:
    ldr	pc, not_used_handle
    bx lr
    not_used_handle:.word not_used
    
__irq:
    ldr	pc, irq_handle
    bx lr
    irq_handle:.word irq
    
__fiq:
    ldr r0, =0x0101f1000
	str r0, [r0]
    ldr	pc, fiq_handle
    bx lr
    fiq_handle:.word fiq

