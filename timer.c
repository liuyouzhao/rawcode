#include "versatile.h"

#define TIMER_LOAD_VAL 0xffffffff

#define READ_TIMER (*(volatile ulong *)(CONFIG_SYS_TIMERBASE+4))

#define TIMER_ENABLE	(1 << 7)
#define TIMER_MODE_MSK	(1 << 6)
#define TIMER_MODE_FR	(0 << 6)
#define TIMER_MODE_PD	(1 << 6)

#define TIMER_INT_EN	(1 << 5)
#define TIMER_PRS_MSK	(3 << 2)
#define TIMER_PRS_8S	(1 << 3)
#define TIMER_SIZE_MSK	(1 << 2)
#define TIMER_ONE_SHT	(1 << 0)

typedef unsigned long ulong;

ulong lastdec;
ulong timestamp;

int timer_init (void)
{
	ulong	tmr_ctrl_val;

	/* 1st disable the Timer */
	tmr_ctrl_val = *(volatile ulong *)(CONFIG_SYS_TIMERBASE + 8);
	tmr_ctrl_val &= ~TIMER_ENABLE;
	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 8) = tmr_ctrl_val;

	/*
	 * The Timer Control Register has one Undefined/Shouldn't Use Bit
	 * So we should do read/modify/write Operation
	 */

	/*
	 * Timer Mode : Free Running
	 * Interrupt : Disabled
	 * Prescale : 8 Stage, Clk/256
	 * Tmr Siz : 16 Bit Counter
	 * Tmr in Wrapping Mode
	 */
	tmr_ctrl_val = *(volatile ulong *)(CONFIG_SYS_TIMERBASE + 8);
	tmr_ctrl_val &= ~(TIMER_MODE_MSK | TIMER_PRS_MSK | TIMER_SIZE_MSK | TIMER_ONE_SHT );
	tmr_ctrl_val |= TIMER_INT_EN;
	tmr_ctrl_val |= (TIMER_ENABLE | TIMER_PRS_8S);

	*(volatile ulong *)(CONFIG_SYS_TIMERBASE + 8) = tmr_ctrl_val;

	/* reset time */
	lastdec = READ_TIMER;  /* capure current decrementer value time */
	timestamp = 0;	       /* start "advancing" time stamp from 0 */

	return 0;
}

