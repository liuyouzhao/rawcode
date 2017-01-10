#include <config.h>
#include <irq.h>

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

static irq_vec_t *s_irq_vecs = 0;

void arch_set_irq_table(irq_vec_t *vec)
{
    s_irq_vecs = vec;
}

FUNC_ASMORG void irq()
{
    int n = 0;
    int irq_stats = getreg32(CONF_VIC_BASE);

    __DEBUG__

    void (*vec)();
    void (*ack)();
    for( ; n < VIC_NUM; n ++ ) {
        if(irq_stats & (0x1 << n)) {

            /* call ack func */
            if(ack = _PTRFV_(_ARR_(&s_irq_acks)[n])) {
                ack();            
            }

            /* dispatch irq */
            if(s_irq_vecs && (vec = _PTRFV_(_ARR_(s_irq_vecs)[n]))) {
                vec();            
            }
        }
    }
}
