.code 32

.global asm_task_save_context
asm_task_save_context:
    PUSH    {lr}

    @; Get last registers
    LDR     r9, =0x14000
    LDR     r8, [r9]
    LDR     r1, [r9, #-4]
    LDR     r2, [r9, #-4*2]
    LDR     r3, [r9, #-4*3]
    LDR     r4, [r9, #-4*4]
    LDR     r5, [r9, #-4*5]
    LDR     r6, [r9, #-4*6]
    LDR     r7, [r9, #-4*7]
    LDR     lr, [r9, #-4*8] @; lr
    LDR     r5, [r9, #-4*9] @; cpsr

    @; Save all common registers
    STR     r8,  [r0]
    STR     r1,  [r0, #4]
    STR     r2,  [r0, #4*2]
    STR     r3,  [r0, #4*3]
    STR     r4,  [r0, #4*4]
    STR     r5,  [r0, #4*5]
    STR     r6,  [r0, #4*6]
    STR     r7,  [r0, #4*7]
    STR     lr,  [r0, #4*15]  @; Save lr to pc
    STR     r5,  [r0, #4*16]  @; Save cpsr

    @; Switch to svc mode, check all regs last task use
    MRS     r1, cpsr
    BIC     r1, r1, #0x1F     @; clear mode field
    ORR     r1, r1, #0x13     @; user mode code
    MSR     cpsr_c, r1     

    @; Save LR
    STR     r10, [r0, #4*10]
    STR     r11, [r0, #4*11]
    STR     ip,  [r0, #4*12]    @; Save ip
    STR     sp,  [r0, #4*13]    @; Save sp
    STR     lr,  [r0, #4*14]    @; Save lr

    @; switch to IRQ mode, sp is sp_irq
    MRS     r1, cpsr
    ORR     r1, r1, #0x1f
    AND     r1, #0x12
    MSR     cpsr_c, r1

    POP     {lr}
    BX      lr


.global asm_task_switch_context
asm_task_switch_context:

    @; Switch to svc mode, check all regs last task use
    MRS     r1, cpsr
    ORR     r1, r1, #0x1f
    AND     r1, #0x13
    MSR     cpsr_c, r1

    LDR     r8,  [r0]
    LDR     r1,  [r0, #4]
    LDR     r2,  [r0, #4*2]
    LDR     r3,  [r0, #4*3]
    LDR     r4,  [r0, #4*4]
    LDR     r5,  [r0, #4*5]
    LDR     r6,  [r0, #4*6]
    LDR     r7,  [r0, #4*7]
    LDR     r10, [r0, #4*10]
    LDR     r11, [r0, #4*11]
    LDR     ip,  [r0, #4*12]
    LDR     r7,  [r0, #4*13]  @; Save sp to sp
    LDR     lr,  [r0, #4*14]  @; Save lr to pc
    LDR     r5,  [r0, #4*16]  @; Save cpsr

    /* open IRQ */
    MRS r5, cpsr       /* read cpsr content into r1 */
    BIC r5, r5, #0x80  /* bit clean, clean x000 0000 */
    MSR cpsr_c, r5     /* only set control bits[0-7] */

    LDR     r5, [r0, #4*15]
    LDR     r6,  [r0, #4*16]  @; Load cpsr
    MSR     cpsr, r6
    MOV     sp, r7
    MOV     r0, r8
    subs    pc, r5, #4


.global asm_get_sp
asm_get_sp:
    MOV r0, sp
    mov pc, lr
    
