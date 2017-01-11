#ifndef __INCLUDE_IRQ_H
#define __INCLUDE_IRQ_H

/* TODO: Retreive VIC list */
#define VIC_NUM    5
#define VIC_TIMER0 0x10


typedef struct irq_vec_s
{
    void (*unknown0)();
    void (*unknown1)();
    void (*unknown2)();
    void (*unknown3)();
    void (*sys_tick)();
} irq_vec_t;

typedef struct irq_ack_s
{
    void (*unknown0)();
    void (*unknown1)();
    void (*unknown2)();
    void (*unknown3)();
    void (*sys_tick)();
} irq_ack_t;

void arch_bind_systick(void (*systick)());
void irq();

#endif /* __INCLUDE_IRQ_H */
