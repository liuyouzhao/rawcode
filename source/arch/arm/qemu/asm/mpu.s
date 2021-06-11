/*
===========================================================================
CP15 co-processor
registers:
c0-c15

MMU
===========================================================================
*/
.code 32

.equ	region_32K     , 0b01110
.equ	region_4G      , 0b11111
.equ	region_Enable  , 0b1

@; MPU global setup

.global asm_mpu_config
asm_mpu_config:
    push	{r1-r6, lr}

@; Close irq
    mrs r1, cpsr
    orr r1, r1, #0x80  /* bit clean, 0x0100000 close IRQ */
    MSR cpsr_c, r1

@; Enable mmu
    ldr     r0, tlb_l1_base
    mcr     p15, 0, r0, c2, c0, 0
tlb_l1_base: .word 0x10100c02
    ldr     r0, entry4020
    ldr     r1, val4020
    str     r1, [r0]
entry4020:  .word 0x10101008
val4020:    .word 0x10100c02

    mov     r1, #0x3
    mcr     p15, 0, r1, c3, c0, 0

    mrc     p15, 0, r2, c1, c0, 0       @; read CP15 register 1
    orr     r2, r2, #0x1                @; enable MMU bit
    mcr     p15, 0, r2, c1, c0, 0       @; write cp15 register 1

@; Re-open IRQ
    mrs r1, cpsr
    bic r1, r1, #0x80
    mov r2, r1
    msr cpsr_c, r1
    pop     {r1-r6, pc}

.global asm_get_c1
asm_get_c1:
    push	{r1-r6, lr}

@; Close irq
    mrs r1, cpsr
    orr r1, r1, #0x80  /* bit clean, 0x0100000 close IRQ */
    MSR cpsr_c, r1

    mrc p15, 0, r0, c1, c0, 0

@; Re-open IRQ
    mrs r1, cpsr
    bic r1, r1, #0x80
    mov r2, r1
    msr cpsr_c, r1
    pop     {r1-r6, pc}
