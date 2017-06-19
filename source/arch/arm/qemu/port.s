.code 32

.global asm_task_enter_swi
asm_task_enter_swi:
    push {lr}
    swi 0x123
    pop {lr}
    bx lr


.global asm_task_switch_context
asm_task_switch_context:
    LDR     r1,  [r0, #4]    @; Restore r1
    LDR     r2,  [r0, #4*2]  @; Restore r2
    LDR     r3,  [r0, #4*3]  @; Restore r3
    LDR     r4,  [r0, #4*4]  @; Restore r4
    LDR     r5,  [r0, #4*5]  @; Restore r5
    LDR     r6,  [r0, #4*6]  @; Restore r6
    LDR     r7,  [r0, #4*7]  @; Restore r7
    LDR     r8,  [r0, #4*8]  @; Restore r8
    LDR     r9,  [r0, #4*9]  @; Restore r9
    LDR     r10, [r0, #4*10] @; Restore r10
    LDR     r11, [r0, #4*11] @; Restore r11
    LDR     r12, [r0, #4*12] @; Restore r12
    LDR     sp,  [r0, #4*13]  @; Restore sp
    LDR     lr,  [r0, #4*14]  @; Restore lr

    @; Restore cpsr
    PUSH    {r5}
    LDR     r5,  [r0, #4*16]  @; Save cpsr
    MSR     cpsr, r5
    POP     {r5}

    /* reset tick */
    PUSH    {r5, r6}
    MOV     r5, #0x1
    LDR     r6, IC_TIMER0
    STR     r5, [r6, #4*3]
    POP     {r5, r6}

    /* open IRQ */
    PUSH    {r5}
    MRS r5, cpsr       /* read cpsr content into r1 */
    BIC r5, r5, #0x80  /* bit clean, clean x000 0000 */
    MSR cpsr_c, r5     /* only set control bits[0-7] */
    POP     {r5}

    PUSH    {r5}
    LDR     r5, [r0, #4*15] /* now r5 is pc*/
    STR     r5, __SAVE_P0 /* now __SAVE_P0 is pc */
    LDR     r0, [r0] /* reset r0 */
    POP     {r5}
    LDR     pc, __SAVE_P0
__SAVE_P0: .word g_registers_asm
IC_TIMER0: .word 0x101e2000


.global asm_get_sp
asm_get_sp:
    MOV r0, sp
    MOV pc, lr


/* Must in irq mode */
.global asm_task_save_switch
asm_task_save_context:
    @; Reset sp address
    LDR sp, =isr_stack_top

    SUB       lr, lr, #4       @; modify LR

    PUSH      {r12}     @; store AAPCS registers on to the IRQ mode stack
    LDR       r12, =g_registers_asm
    STMIA     r12!, {r0-r11}
    LDR       r0, =g_registers_asm
    POP       {r12}
    STR       r12, [r0, #4*12]
    STR       lr, [r0, #4*15]

    @; Switch to svc mode, check all regs last task use
    MRS       r1, cpsr
    BIC       r1, r1, #0x1F     @; clear mode field
    ORR       r1, r1, #0x13     @; user mode code
    MSR       cpsr_c, r1     

    STR       sp, [r0, #4*13]
    STR       lr, [r0, #4*14]

