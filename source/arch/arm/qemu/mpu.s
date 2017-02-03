/*
===========================================================================
CP15 co-processor
registers:
c0-c15

MMU
===========================================================================
*/

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
    mcr     p15, 0, r0, c2, c0, 0
    orr     r0, r0, #0b10
    orr     r0, r0, #(0b10 << 10)
    orr     r0, r0, #(0b10000)
    orr     r0, r0, #(0b01 << 5)
    mcr     p15, 0, r0, c2, c0, 0

    mov     r1, #0x3
    mcr     p15, 0, r1, c3, c0, 0

    mrc     p15, 0, r2, c1, c0, 0       @; read CP15 register 1
    orr     r2, r2, #0x0                @; enable MPU bit
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
