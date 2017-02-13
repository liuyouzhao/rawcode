.code 32

.global asm_task_save_context
asm_task_save_context:

    PUSH    {r4}
    LDR     r4, =0x14000

    PUSH    {r0, r1}
    MOV     r1, r0
    LDR     r0, [r4]
    STR     r0, [r1]
    POP     {r0, r1}

    @; save r1 to array
    LDR     r1, [r4, #-4]
    STR     r1,  [r0, #4]
    
    @; save r2 to array
    LDR     r1, [r4, #-4*2]
    STR     r1,  [r0, #4*2]

    @; save r3 to array
    LDR     r1, [r4, #-4*3]
    STR     r1, [r0, #4*3]

    @; save r4 to array
    LDR     r1, [r4, #-4*4]
    STR     r1, [r0, #4*4]

    @; save r5 to array
    LDR     r1, [r4, #-4*5]
    STR     r1, [r0, #4*5]

    @; save r6 to array
    LDR     r1, [r4, #-4*6]
    STR     r1, [r0, #4*6]

    @; save r7 to array
    LDR     r1, [r4, #-4*7]
    STR     r1, [r0, #4*7]

    @; save r8 to array
    LDR     r1, [r4, #-4*8]
    STR     r1, [r0, #4*8]

    @; save r9 to array
    LDR     r1, [r4, #-4*9]
    STR     r1, [r0, #4*9]

    @; save r10 to array
    LDR     r1, [r4, #-4*10]
    STR     r1, [r0, #4*10]

    @; save r11 to array
    LDR     r1, [r4, #-4*11]
    STR     r1, [r0, #4*11]

    @; save r12 to array
    LDR     r1, [r4, #-4*12]
    STR     r1, [r0, #4*12]


    @; save lr to array pc
    PUSH    {lr}
    LDR     lr, [r4, #-4*15]
    STR     lr, [r0, #4*15]
    POP     {lr}

    @; save cpsr
    LDR     r5, [r4, #-4*16]
    STR     r5, [r0, #4*16]  @; Save cpsr

    @; save spsr
    LDR     r5, [r4, #-4*17]
    STR     r5, [r0, #4*17]

    @; Switch to svc mode, check all regs last task use
    MRS     r1, cpsr
    BIC     r1, r1, #0x1F     @; clear mode field
    ORR     r1, r1, #0x13     @; user mode code
    MSR     cpsr_c, r1     

    @; Save lr, sp, now lr_svc, sp_svc
    STR     sp,  [r0, #4*13]    @; Save sp
    STR     lr,  [r0, #4*14]    @; Save lr

    @; switch to IRQ mode, sp is sp_irq, lr is lr_irq
    MRS     r1, cpsr
    ORR     r1, r1, #0x1f
    AND     r1, #0x12
    MSR     cpsr_c, r1

    BX      lr


.global asm_task_switch_context
asm_task_switch_context:
    @; Switch to svc mode, check all regs last task use
    MRS     r1, cpsr
    ORR     r1, r1, #0x1f
    AND     r1, #0x13
    MSR     cpsr_c, r1

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

    @; Get pc to r5 and sub 4
    LDR     r5, [r0, #4*15]
    SUBS    r5, r5, #4
    STR     r5, [r0, #4*15] @; Save back to pc
    POP     {r5}

    PUSH    {r5}
    /* open IRQ */
    MRS r5, cpsr       /* read cpsr content into r1 */
    BIC r5, r5, #0x80  /* bit clean, clean x000 0000 */
    MSR cpsr_c, r5     /* only set control bits[0-7] */
    POP     {r5}

    LDR     r0, [r0, #4*15]
    STR     r0, __SAVE_P0
    LDR     r0, [r0]
    LDR     pc, __SAVE_P0
__SAVE_P0: .word 0x14000

.global asm_get_sp
asm_get_sp:
    MOV r0, sp
    mov pc, lr
    
