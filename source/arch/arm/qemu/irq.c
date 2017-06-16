#include <arch.h>
#include <irq.h>
#include <utils/util.h>
#include <port/port.h>

static void none() {}
extern void arch_tick_done();

static irq_ack_t s_irq_acks =
{
    .unknown0 = 0,
    .unknown1 = 0,
    .unknown2 = 0,
    .unknown3 = 0,
    .sys_tick = arch_tick_done
};

static irq_vec_t s_irq_vecs =
{
    .unknown0 = 0,
    .unknown1 = 0,
    .unknown2 = 0,
    .unknown3 = 0,
    .sys_tick = 0
};

void arch_bind_systick(void (*systick)())
{
    s_irq_vecs.sys_tick = systick;
}

extern unsigned int g_registers_asm[18];
void irq()
{
    int n = 0;
    int irq_stats = getreg32(IC_VIC_BASE);
    
    //kprintf("irq:%p %p %p\n", g_registers_asm[0], g_registers_asm[1], g_registers_asm[2]);

    void (*vec)();
    void (*ack)();
    for( ; n < VIC_NUM; n ++ ) {
        if(irq_stats & (0x1 << n)) {

            /* call ack func */
            if(ack = _PTRFV_(_ARR_(&s_irq_acks)[n])) {
                ack();            
            }

            /* dispatch irq */
            if((vec = _PTRFV_(_ARR_(&s_irq_vecs)[n]))) {
                vec();
            }
        }
    }
}
