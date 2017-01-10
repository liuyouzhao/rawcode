/* Command */
#define _PTR_(a)              (volatile unsigned int *)(a)

/* Registers control */
#define getreg32(a)          (*(volatile int *)(a))
#define putreg32(v,a)        (*(volatile int *)(a) = (v))


#define printf uart0_printf_ext

#define __DEBUG__ printf("%s [%s:%d]\n", __FILE__, __FUNCTION__, __LINE__);


struct __registers__
{
    int r[8];
    int sp;
    int lr;
    int pc;
    int cpsr;
};

