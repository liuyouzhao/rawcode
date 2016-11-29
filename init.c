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

void uart0_printhex32(int hex)
{
    int i = 0;
    int l = 0;
    int r = 0;
    *(_p_uart0_start_) = '0';
    *(_p_uart0_start_) = 'x';
    for(i = 3; i >= 0; i -= 1)
    {
        l = (((hex >> (i * 8)) & 0xff) >> 4);
        r = (((hex >> (i * 8)) & 0xff) % 16);
        l = l > 9 ? l + 55 : l + 48;
        r = r > 9 ? r + 55 : r + 48;

        *_p_uart0_start_ = l;
        *_p_uart0_start_ = r;
    }
    *(_p_uart0_start_) = '\n';
}


void reset()
{
	uart0_printf("reset\n");
}

void undefined_instruction()
{
    uart0_printf("111\n");
}

void software_interrupt(unsigned int inst, unsigned int sp)
{
    uart0_printf("[software_interrupt]\n");
    uart0_printhex32(inst);
    uart0_printhex32(sp);
    uart0_printhex32(_query_code(sp + 0x0));
    uart0_printhex32(_query_code(sp + 0x4));
    uart0_printhex32(_query_code(sp + 0x8));
    uart0_printf("[software_interrupt end]\n");
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
    uart0_printf("irq interrupt came!\n");
}

void fiq()
{
    uart0_printf("777\n");
}

int steven()
{
    int begin = 0x0;
    int cmd = 0x0;
    
    uart0_printf("steven111\n");

    
    for(begin = 0x0; begin < 0x80; begin += 0x4)
    {
        cmd = _query_code(0x10000 + begin);
        _insert_code(cmd, begin);
        uart0_printhex32(_query_code(begin));
    }

    //_test();
    //do_swi();

    uart0_printf("steven222\n");
    
    timer_init();

    tick_loop(uart0_printf);

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


