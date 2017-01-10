#include <utils/util.h>
#include <irq.h>

static void rc_sys_tick();

static irq_vec_t s_irq_vec =
{
    .unknown0 = 0,
    .unknown1 = 0,
    .unknown2 = 0,
    .unknown3 = 0,
    .sys_tick = rc_sys_tick
};

static void rc_sys_tick()
{
    __DEBUG__
}

int rc_task_init()
{
    arch_set_irq_table(&s_irq_vec);
}
