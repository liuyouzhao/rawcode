.code 32

.global asm_task_save_context
asm_task_save_context:
    MOV     r5, lr


    @; r0 is param regs, save to r13
    MOV     r4, r0

    @; Get last registers
    POP     {r0-r3, lr}


    @; Save all common registers
    STR     r1,  [r4]
    STR     r2,  [r4, #4]
    STR     r3,  [r4, #4*2]
    STR     lr,  [r4, #4*15]

    @; Switch to svc mode, check all regs last task use
    MRS     r1, cpsr
    BIC    r1, r1, #0x1F     @; clear mode field
    ORR    r1, r1, #0x13     @; user mode code
    MSR     cpsr_c, r1     

    @; Save LR, PC
    STR     sp,  [r4, #4*13]    @; Save sp to sp
    STR     lr,  [r4, #4*14]    @; Save lr to lr

    @; switch to IRQ mode, sp is sp_irq
    MRS     r1, cpsr
    ORR     r1, r1, #0x1f
    AND     r1, #0x12
    MSR     cpsr_c, r1     

    MOV     pc, r5
