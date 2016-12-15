#include <unistd.h>
#include <stdarg.h>

struct __registers__
{
    int r[8];
    int sp;
    int lr;
    int pc;
    int cpsr;
};

volatile unsigned char* _p_uart0_start_ = (unsigned char*) 0x0101f1000;

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

void uart0_printnum10(int num)
{
    int i = num;
    int d = 0;
    char arr[12] = {0};
    char* p = arr + 11;
    
    while(i > 0) {
        arr[d] = (i % 10) + 48;
        d ++;
        i = i / 10;
    }

    while(*p == 0) {
        p --;
    }

    for(i = 0; i < d; i ++) {
        *_p_uart0_start_ = *p;
        p --;
    }
}

void uart0_printf(const char* text)
{
    char* tmp = (char*) text;
    while((*tmp) != '\0')
    {
        *_p_uart0_start_ = *tmp;
        tmp ++;
    }
}

void uart0_printf_ext(const char* text, ...)
{
    char* tmp = (char*) text;
    char close = 0;
    int n = 0;
    char c = 0;
    char* pstr = 0;
    double f = 0.0f;
        
    va_list ap;  
    va_start(ap, text);  

    while((*tmp) != '\0')
    {
        if(*tmp == '%' && *(tmp + 1) == 'd') {
            n = va_arg(ap, int);
            uart0_printnum10(n);
        } else if(*tmp == '%' && *(tmp + 1) == 'c') {
            c = va_arg(ap, int);
            *_p_uart0_start_ = c;
        } else if(*tmp == '%' && *(tmp + 1) == 's') {
            pstr = va_arg(ap, char*);
            uart0_printf(pstr);
        } else if(*tmp == '%' && *(tmp + 1) == 'f') {
            f = va_arg(ap, double);
            /// Not Implement
        }
        tmp ++;
    }
    va_end(ap);
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
    tick_done();
    uart0_printf("666\n");
    uart0_printf("irq interrupt came!\n");
    uart0_printf("switch to next thread...\n");
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


