#include <config.h>
#include <timer.h>
#include <irq.h>

#include <utils/util.h>
#define VIC_INT_ENABLE (volatile unsigned int*)(CONF_VIC_BASE + VIC_TIMER0)
#define VIC_INTENABLE 0x4 /* 0x10 bytes */

int boot_timer_init()
{
    *(VIC_INT_ENABLE) |= (1 << 4) | (1 << 5);
	*_PTR_(_PTR_(CONF_VIC_BASE) + VIC_INTENABLE) = VIC_TIMER0;
    *_PTR_(CONF_TIMER0) = 1000 * 100;
    *_PTR_(_PTR_(CONF_TIMER0) + TIMER_CONTROL) = TIMER_EN | TIMER_PERIODIC | TIMER_32BIT | TIMER_INTEN;
	return 0;
}


int boot_tick_done()
{
    if(*(_PTR_(CONF_TIMER0) + TIMER_MIS)) {
        *(_PTR_(CONF_TIMER0) + TIMER_INTCLR) = 1;
    }
}
