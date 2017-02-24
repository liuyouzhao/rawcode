#include <utils/debug.h>
#include <port/port.h>

static void dbg_printhex32(int hex)
{
    int i = 0;
    int l = 0;
    int r = 0;
    *(glb_output_uart_addr) = '0';
    *(glb_output_uart_addr) = 'x';
    for(i = 3; i >= 0; i -= 1)
    {
        l = (((hex >> (i * 8)) & 0xff) >> 4);
        r = (((hex >> (i * 8)) & 0xff) % 16);
        l = l > 9 ? l + 55 : l + 48;
        r = r > 9 ? r + 55 : r + 48;

        *glb_output_uart_addr = l;
        *glb_output_uart_addr = r;
    }
}

static void dbg_printnum10(int num)
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

    if(d == 0) {
        *glb_output_uart_addr = '0';
        return;
    }
    for(i = 0; i < d; i ++) {
        *glb_output_uart_addr = *p;
        p --;
    }
}

void dbg_printf(const char* text)
{
    char* tmp = (char*) text;
    while((*tmp) != '\0')
    {
        *glb_output_uart_addr = *tmp;
        tmp ++;
    }
}

void dbg_printf_ext(const char* text, ...)
{
    char* tmp = (char*) text;
    char close = 0;
    int n = 0;
    char c = 0;
    char* pstr = 0;
    double f = 0.0f;

    if(g_pt == 0) {
        return;
    }

    rc_task_enter_section();

    va_list ap;
    va_start(ap, text);  

    while((*tmp) != '\0')
    {
        if(*tmp == '%' && (*(tmp + 1) == 'x' || *(tmp + 1) == 'p')) {
            n = va_arg(ap, int);
            dbg_printhex32(n);
            tmp ++;
        }
        else if(*tmp == '%' && *(tmp + 1) == 'd') {
            n = va_arg(ap, int);
            dbg_printnum10(n);
            tmp ++;
        } else if(*tmp == '%' && *(tmp + 1) == 'c') {
            c = va_arg(ap, int);
            *glb_output_uart_addr = c;
            tmp ++;
        } else if(*tmp == '%' && *(tmp + 1) == 's') {
            pstr = va_arg(ap, char*);
            dbg_printf(pstr);
            tmp ++;
        } else if(*tmp == '%' && *(tmp + 1) == 'f') {  
            /// Not Implement
            *glb_output_uart_addr = *tmp;
        }
        else {
            *glb_output_uart_addr = *tmp;
        }
        tmp ++;
    }
    va_end(ap);

    rc_task_try_switch();
}

void dbg_printf_ext_kernel(const char* text, ...)
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
        if(*tmp == '%' && (*(tmp + 1) == 'x' || *(tmp + 1) == 'p')) {
            n = va_arg(ap, int);
            dbg_printhex32(n);
            tmp ++;
        }
        else if(*tmp == '%' && *(tmp + 1) == 'd') {
            n = va_arg(ap, int);
            dbg_printnum10(n);
            tmp ++;
        } else if(*tmp == '%' && *(tmp + 1) == 'c') {
            c = va_arg(ap, int);
            *glb_output_uart_addr = c;
            tmp ++;
        } else if(*tmp == '%' && *(tmp + 1) == 's') {
            pstr = va_arg(ap, char*);
            dbg_printf(pstr);
            tmp ++;
        } else if(*tmp == '%' && *(tmp + 1) == 'f') {  
            /// Not Implement
            *glb_output_uart_addr = *tmp;
        }
        else {
            *glb_output_uart_addr = *tmp;
        }
        tmp ++;
    }
    va_end(ap);
}
