.code 32

/* _start entry
 * vector interrupt table:
 * 0x00: ldr pc, _reset
 * 0x04: ldr pc, _software_interrupt
 * .....
*/
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
	
	
/* enable irq
 * mrs: read from cpsr/spsr
 * c[control]   psr[0-7]
 * x[extension] psr[8-15]
 * s[statement] psr[16-23]
 * f[flag]      psr[24-31]
 *
 * The Status register:
 * cpsr[current processor status register]
 * spsr[saved processor status register]

 * |31 30 29 28    7  6  5   4  3  2  1  0|
 * | N  Z  C  V....I  F  T  M4 M3 M2 M1 M0|
 * [N] save as calc-res bit[31],1(>=0),0(<0)
 * [Z] calc-res of CMP, 1(=0),0(!=0)
 * [C] sum&sub,carrybit or borrowbit
 * cpsr_c: 0-7
 *
 * *** M[0-4] ***
 * M0-M4   mode       accessable
 * ob10000 user       pc,r14~r0,CPSR
 * 0b10001 FIQ        PC,R14_FIQ-R8_FIQ,R7~R0,CPSR,SPSR_FIQ
 * 0b10010 IRQ        PC,R14_IRQ-R13_IRQ,R12~R0,CPSR,SPSR_IRQ
 * 0B10011 SUPERVISOR PC,R14_SVC-R13_SVC,R12~R0,CPSR,SPSR_SVC
 * 0b10111 ABORT      PC,R14_ABT-R13_ABT,R12~R0,CPSR,SPSR_ABT
 * 0b11011 UNDEFINEED PC,R14_UND-R8_UND,R12~R0,CPSR,SPSR_UND
 * 0b11111 SYSTEM     PC,R14-R0,CPSR(ARM v4+)
*/
__reset:
    LDR sp, =stack_top

     /* open IRQ */
    MRS r1, cpsr       /* read cpsr content into r1 */
    BIC r1, r1, #0x80  /* bit clean, clean x000 0000 */
    mov r2, r1         /* backup to r2, why? */
    MSR cpsr_c, r1     /* only set control bits[0-7] */
    and r1, #0x12
    MSR cpsr_c, r1

     /* close FIQ */
    MRS r1, cpsr
    ORR r1, r1, #0x40  /* bit clean, 0x00 0000 close FIQ */
    MSR cpsr_c, r1

    LDR sp, =isr_stack_top

    and r1, #0x13
    MSR cpsr_c, r1
    LDR sp, =svc_stack_top

    MSR cpsr_c, r2

    BL steven
    B .

    
    
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
    
/*    ldr r0,[lr,#-4]*/
/*    mov r1,sp */
__irq:
    push {lr}
    bl irq
    pop {lr}
    subs pc,lr,#4
    
    
 /*   subs pc, lr, #4
    mov r9, lr
    bl irq*/
/*    ldr	pc, irq_handle */
/*    irq_handle:.word irq */
    
/*__irq_revert:
    mov r14, r13
    subs pc, r14, #4
*/


__fiq:
    ldr	pc, fiq_handle
    bx lr
    fiq_handle:.word fiq

