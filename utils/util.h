#ifndef __INCLUDE_UTIL_H
#define __INCLUDE_UTIL_H

/* Command */
/* Mark as organized with asm functions */
#define FUNC_ASMORG

/* Mark as organized with core functions */
#define FUNC_COREORG

/* Addres opt */
#define _PTR_(a)              (volatile unsigned int *)(a)
#define _ARR_(a)              ((volatile unsigned int *)(a))
#define _PTRFV_(a)            (void (*)())(a)

/* Registers control */
#define getreg32(a)          (*(volatile int *)(a))
#define putreg32(v,a)        (*(volatile int *)(a) = (v))


#define printf uart0_printf_ext

#define __DEBUG__ printf("%s [%s:%d]\n", __FILE__, __FUNCTION__, __LINE__);

#endif /* __INCLUDE_UTIL_H */
