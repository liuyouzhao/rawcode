#include <unistd.h>

struct __registers__
{
    int r[8];
    int sp;
    int lr;
    int pc;
    int cpsr;
};

volatile struct __registers__* _p_registers_ = (struct __registers__*) 0x10000000; 
volatile unsigned char* _p_uart0_start_ = (unsigned char*) 0x0101f1000;
void uart0_printf(const char* text)
{
    char* tmp = (char*) text;
    while((*tmp) != '\0')
    {
        *_p_uart0_start_ = *tmp;
        tmp ++;
    }
}
/*
    ldr pc, _reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort //12
	ldr	pc, _data_abort  //16
	ldr	pc, _not_used  //20
	ldr	pc, _irq  //24
	ldr	pc, _fiq  //28
*/


volatile int (*_reset) = (int*) 0x10140000;
volatile int (*_undefined_instruction) = 0x10140000 + 4;
volatile int (*_software_interrupt) = 0x10140000 + 8;
volatile int (*_prefetch_abort) = 0x10140000 + 12;
volatile int (*_data_abort) = 0x10140000 + 16;
volatile int (*_not_used) = 0x10140000 + 20;
volatile int (*_irq) = 0x10140000 + 24;
volatile int (*_fiq) = 0x10140000 + 28;


void reset()
{
	uart0_printf("reset\n");
}

void undefined_instruction()
{
    uart0_printf("111\n");
}

void software_interrupt()
{
    uart0_printf("222\n");
}

void prefetch_abort()
{
    uart0_printf("333\n");
}

void data_abort()
{
    uart0_printf("444\n");
}

void not_used()
{
    uart0_printf("555\n");
}

void irq()
{
    uart0_printf("666\n");
}

void fiq()
{
    uart0_printf("777\n");
}




int steven()
{
    uart0_printf("steven\n");
#if 1
	*_reset = reset;
	*_undefined_instruction = undefined_instruction;
	*_software_interrupt = software_interrupt;
	*_prefetch_abort = prefetch_abort;
	*_data_abort = data_abort;
    *_not_used = not_used;
	*_irq = irq;
    *_fiq = fiq;
#endif
    timer_init();
    uart0_printf("steven222\n");

	while(1);

    return 0;
}

void pthread_1()
{
    while(1)
    {
        uart0_printf("pthread_1\n");
    }
}

void pthread_2()
{
    while(1)
    {
        uart0_printf("pthread_2\n");
    }
}


#define TIMER0_BASE 0x101e2000

int kernel()
{
    //*((int*)(_p_registers_->lr)) = (int) pthread_1; 

     

    asm(
        "B pthread_1;"
    );

//    while(1)
    {
        uart0_printf("kernel while..\n");
    }
    return 0;
}

int timer = 0;
void Timer0_Handle(void)
{
    //char buf[32] = {0};
    //sprintf(buf, "timer: %d\n", timer);
    //uart0_printf(buf);
    uart0_printf("timer0---->\n");
}



