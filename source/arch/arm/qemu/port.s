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
    LDR     sp,  [r0, #4*13]  @; Save sp to sp
    LDR     lr,  [r0, #4*14]  @; Save lr to pc

    @; Restore cpsr
    PUSH    {r5}
    LDR     r5,  [r0, #4*16]  @; Save cpsr
    MSR     cpsr, r5
    POP     {r5}

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



.global asm_get_sp
asm_get_sp:
    MOV r0, sp
    MOV pc, lr


.global asm_task_save_switch
asm_task_save_switch:
    STR     r0,  [r0]
    STR     r1,  [r0, #4]
    STR     r2,  [r0, #4*2]
    STR     r3,  [r0, #4*3]
    STR     r4,  [r0, #4*4]
    STR     r5,  [r0, #4*5]
    STR     r6,  [r0, #4*6]
    STR     r7,  [r0, #4*7]
    STR     r8,  [r0, #4*8]
    STR     r9,  [r0, #4*9]
    STR     r10,  [r0, #4*10]
    STR     r11,  [r0, #4*11]
    STR     r12,  [r0, #4*12]
    STR     r13,  [r0, #4*13]
    STR     r14,  [r0, #4*14]
    STR     r15,  [r0, #4*15]
    MRS     r5, cpsr
    STR     r5,  [r0, #4*16]
    MRS     r5, spsr
    STR     r5,  [r0, #4*17]
    MOV     r0, r1
    B       asm_task_switch_context

