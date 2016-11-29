.code 32

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
     LDR sp, =stack_top
     
     MRS r1, cpsr
     BIC r1, r1, #0x80
     mov r2, r1
     MSR cpsr_c, r1

     and r1, #0x12
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
    
__irq:
    /* TODO: do all registers processing steps */
    ldr	pc, irq_handle
    bx lr
    irq_handle:.word irq
    
__fiq:
    ldr	pc, fiq_handle
    bx lr
    fiq_handle:.word fiq

