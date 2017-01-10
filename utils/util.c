#include <stdarg.h>
#include <config.h>

volatile static unsigned char* _p_uart0_start_ = (unsigned char*) CONF_UART0;

/********************************
 * Register base functions
********************************/




/*****************************
 * uart0 printf
********************************/
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
        if(*tmp == '%' && *(tmp + 1) == 'x') {
            n = va_arg(ap, int);
            uart0_printhex32(n);
            tmp ++;
        }
        else if(*tmp == '%' && *(tmp + 1) == 'd') {
            n = va_arg(ap, int);
            uart0_printnum10(n);
            tmp ++;
        } else if(*tmp == '%' && *(tmp + 1) == 'c') {
            c = va_arg(ap, int);
            *_p_uart0_start_ = c;
            tmp ++;
        } else if(*tmp == '%' && *(tmp + 1) == 's') {
            pstr = va_arg(ap, char*);
            uart0_printf(pstr);
            tmp ++;
        } else if(*tmp == '%' && *(tmp + 1) == 'f') {  
            /// Not Implement
            *_p_uart0_start_ = *tmp;
        }
        else {
            *_p_uart0_start_ = *tmp;
        }
        tmp ++;
    }
    va_end(ap);
}

