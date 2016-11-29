
#define PIC ((volatile unsigned int*)0x10140000)
#define PIC_INT_ENABLE (volatile unsigned int*)0x10140010
#define PIC_TIMER01 0x10
#define VIC_INTENABLE 0x4 /* 0x10 bytes */


#define TIMER0 ((volatile unsigned int*)0x101E2000)
#define TIMER_VALUE 0x1 /* 0x04 bytes */
#define TIMER_CONTROL 0x2 /* 0x08 bytes */
#define TIMER_INTCLR 0x3 /* 0x0C bytes */
#define TIMER_MIS 0x5 /* 0x14 bytes */
#define TIMER_EN 0x80
#define TIMER_PERIODIC 0x40
#define TIMER_INTEN 0x20
#define TIMER_32BIT 0x02
#define TIMER_ONESHOT 0x01

typedef int ulong;

ulong lastdec;
ulong timestamp;

int timer_init (void)
{
    *(PIC_INT_ENABLE) |= (1 << 4) | (1 << 5);
	*(PIC + VIC_INTENABLE) = PIC_TIMER01;
    *TIMER0 = 1000000;
    *(TIMER0 + TIMER_CONTROL) = TIMER_EN | TIMER_PERIODIC | TIMER_32BIT | TIMER_INTEN;
	return 0;
}

int tick_loop(void (*printf)(const char*))
{
    while(1)
    {

        if(*(TIMER0 + TIMER_MIS)) {
	        *(TIMER0 + TIMER_INTCLR) = 1;
	        do_swi();
	        printf("tick\n");
        }
      
    }
    
}
