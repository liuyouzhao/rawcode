#include <arch.h>
#include <timer.h>
#include <irq.h>

#include <port/port.h>
#include <utils/util.h>

#define VIC_INT_ENABLE (volatile unsigned int*)(IC_VIC_BASE + VIC_TIMER0)
#define VIC_INTENABLE 0x4 /* 0x10 bytes */
#define TICK_MS 10

int arch_tick_init()
{
    *(VIC_INT_ENABLE) |= (1 << 4) | (1 << 5);
	*_PTR_(_PTR_(IC_VIC_BASE) + VIC_INTENABLE) = VIC_TIMER0;
    *_PTR_(IC_TIMER0) = 1000 * TICK_MS;
    *_PTR_(_PTR_(IC_TIMER0) + TIMER_CONTROL) = TIMER_EN | TIMER_PERIODIC | TIMER_32BIT | TIMER_INTEN;
	return 0;
}

int arch_tick_enable()
{
    *_PTR_(_PTR_(IC_TIMER0) + TIMER_CONTROL) |= TIMER_EN;
}

void arch_tick_disable()
{
    *_PTR_(_PTR_(IC_TIMER0) + TIMER_CONTROL) &= ~0x80;
}

void arch_tick_done()
{
    if(*(_PTR_(IC_TIMER0) + TIMER_MIS)) {
        *(_PTR_(IC_TIMER0) + TIMER_INTCLR) = 1;
    }
    g_pt->global_tick ++;
}
