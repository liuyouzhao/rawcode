.code 32

/*
=======================================================================================
Usr         System         Supervisor      Abort         Undefined         IRQ           FIQ
----------------------------------------------------------------------------------------------------------------
R0          R0             R0              R0            R0                R0            R0
R1          R1             R1              R1            R1                R1            R1
R2          R2             R2              R2            R2                R2            R2         
R3          R3             R3              R3            R3                R3            R3
R4          R4             R4              R4            R4                R4            R4
R5          R5             R5              R5            R5                R5            R5
R6          R6             R6              R6            R6                R6            R6
R7          R7             R7              R7            R7                R7            R7
---------------------------------------------------------------------------------------------------------------
R8          R8             R8              R8            R8                R8            R8_fiq
R9/sb       R9             R9              R9            R9                R9            R9_fiq
R10/sl      R10            R10             R10           R10               R10           R10_fiq
R11/fp      R11            R11             R11           R11               R11           R11_fiq
R12/ip      R12            R12             R12           R12               R12           R12_fiq
R13/sp      R13            R13_svc         R13_abt       R13_und           R13_irq       R13_fiq
R14/lr      R14            R14_svc         R14_abt       R14_und           R14_irq       R14_fiq
PC          PC             PC              PC            PC                PC            PC
CPSR        CPSR           CPSR            CPSR          CPSR              CPSR          CPSR
                           SPSR_svc        SPSR_abt      SPSR_und          SPSR_irq      SPSR_fiq
=======================================================================================
R0-R7: all modes the same
R8-R14: diff mode in diff physical register, fiq will be faster
R14/lr: each interrupt mode takes its own R14 to cache return address(+4)
R14_irq: when IRQ happens, R14 will remain unchanged from usr/system mode,
         R14_irq saves usr/system address(+4), once enable irq when irq happens,
         R14_irq will be covered.

R15/PC:
-------------------------------------------
| address   |    code     | state         |
| PC-8      | LDR R0, PC  | Running       |
| PC-4      | ???         | Translating   |
| PC        | ???         | Picking       |
-------------------------------------------

*/

/*
======== R0-R4 Register Usage ===================
** Function Call <= 4 ************************************
str lr, [sp, #-4]! // push current lr to sp pointed stack, let sp-=4
ldr r0, =0x01      // param1
ldr r1, =0x02      // param2
ldr r2, =0x03      // param3
ldr r3, =0x04      // param4
bl func_c          // jump function
ldr lr, [sp], #4   // pop from sp pointed stack

** Function Call > 4 ************************************
str lr, [sp, #-4]!
ldr r0, =0x01      // param1
ldr r1, =0x02      // param2
ldr r2, =0x03      // param3
ldr r3, =0x04      // param4

ldr r4, =0x08      // param8
str r4, [sp, #-4]! // push param8 into sp pointed stack, let sp-=4
ldr r4, =0x07      // param7
str r4, [sp, #-4]!
ldr r4, =0x06      // param6
str r4, [sp, #-4]!
ldr r4, =0x05      // param5
str r4, [sp, #-4]!

bl func_c          // jump function

add sp, sp, #4     // let sp+=4
add sp, sp, #4     // let sp+=4
add sp, sp, #4     // let sp+=4
add sp, sp, #4     // let sp+=4

ldr lr, [sp], #4   // pop from sp pointed stack
=================================================



*/

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
 * ================= The Status register ============================
 * cpsr[current processor status register]
 * spsr[saved processor status register]
 * |31 30 29 28 ... 24 ...  7  6  5   4  3  2  1  0|
 * | N  Z  C  V     J       I  F  T  M4 M3 M2 M1 M0|
 * [N] save as calc-res bit[31],1(>=0),0(<0)
 * [Z] calc-res of CMP, 1(=0),0(!=0)
 * [C] sum&sub,carrybit or borrowbit
 * cpsr_c: 0-7
 *
 * *** M[0-4] ***
 * M0-M4   mode       accessable
 * 0b10000 user       pc,r14~r0,CPSR
 * 0b10001 FIQ        PC,R14_FIQ-R8_FIQ,R7~R0,CPSR,SPSR_FIQ
 * 0b10010 IRQ        PC,R14_IRQ-R13_IRQ,R12~R0,CPSR,SPSR_IRQ
 * 0B10011 SUPERVISOR PC,R14_SVC-R13_SVC,R12~R0,CPSR,SPSR_SVC
 * 0b10111 ABORT      PC,R14_ABT-R13_ABT,R12~R0,CPSR,SPSR_ABT
 * 0b11011 UNDEFINEED PC,R14_UND-R8_UND,R12~R0,CPSR,SPSR_UND
 * 0b11111 SYSTEM     PC,R14-R0,CPSR(ARM v4+)
 *
 * T&J: Instruction set flags
 * |-----------------------------|
 * | J | T | Instruction set     |
 * |-----------------------------|
 * | 0 | 0 | Arm (standard)      |
 * | 0 | 1 | Thumb               |
 * | 1 | 0 | Jazelle             |
 * | 1 | 1 | Reserved            |
 * |-----------------------------|
 *
*/
__reset:
    LDR sp, =stack_top /* sp is normal R13, set normal stack_top */

     /* open IRQ */
    MRS r1, cpsr       /* read cpsr content into r1 */
    BIC r1, r1, #0x80  /* bit clean, clean x000 0000 */
    mov r2, r1         /* backup to r2 */
    MSR cpsr_c, r1     /* only set control bits[0-7] */
    
    /* switch to IRQ mode */
    mrs     r1, cpsr
    orr     r1, r1, #0x1f
    and     r1, #0x12
    MSR cpsr_c, r1     /* switch to IRQ mode, sp is sp_irq */
    LDR sp, =isr_stack_top /* set sp_irq to isr_stack_top */

    /* close FIQ */
    MRS r1, cpsr
    ORR r1, r1, #0x40  /* bit clean, 0x00 0000 close FIQ */
    MSR cpsr_c, r1

    /* switch to supervisor mode */
    mrs     r1, cpsr
    orr     r1, r1, #0x1f
    and     r1, #0x13
    MSR     cpsr_c, r1
    LDR sp, =svc_stack_top /* set sp_irq to svc_stack_top */

    MSR cpsr_c, r2

    BL init
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
    bl undefined_instruction
    
__software_interrupt:
    push {r0, r1, r2, lr}
    MRS r1, cpsr
    MOV r2, r1
    ORR r1, #0x80
    MSR cpsr_c, r1
    bl software_interrupt
    MSR cpsr_c, r2
    pop {r0, r1, r2, lr}
    subs pc,lr,#4
    
__prefetch_abort:
    bl prefetch_abort
    
__data_abort:
    bl data_abort
    
__not_used:
    bl not_used
    
/*    ldr r0,[lr,#-4]*/
/*    mov r1,sp */
__irq:
    push {r0, r1, r2, lr}
    MRS r1, cpsr
    MOV r2, r1
    ORR r1, #0x80
    MSR cpsr_c, r1
    bl irq
    MSR cpsr_c, r2
    pop {r0, r1, r2, lr}
    subs pc,lr,#4
    
__fiq:
    ldr	pc, fiq_handle
    bx lr
    fiq_handle:.word fiq


.global switch_user
switch_user:
    LDR    R0, =stack_top    @; context switch and branch
    MOV    R1, LR
    MSR    CPSR_c, #0x10     @; store modified CPSR into SPSR
    MOV    SP, R0
    MOV    PC, R1

.global switch_svc
switch_svc:
    MRS    R0, CPSR          @; load CPSR into R0
    BIC    R0, R0, #0x1F     @; clear mode field
    ORR    R0, R0, #0x13     @; user mode code
    MSR    SPSR, R0          @; store modified CPSR into SPSR
    MOVS   PC, LR            @; context switch and branch

.global switch_irq
switch_irq:
    MRS    R0, CPSR          @; load CPSR into R0
    BIC    R0, R0, #0x1F     @; clear mode field
    ORR    R0, R0, #0x12     @; user mode code
    MSR    SPSR, R0          @; store modified CPSR into SPSR
    MOVS   PC, LR            @; context switch and branch

