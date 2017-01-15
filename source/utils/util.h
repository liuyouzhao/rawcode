#ifndef __INCLUDE_UTIL_H
#define __INCLUDE_UTIL_H

/* Command */
/* Mark as organized with asm functions */
#define FUNC_ASMREF

/* Mark as organized with core functions */
#define FUNC_COREREF

/* Mark as reference by boot */
#define FUNC_BOOTREF

/* Addres opt */
#define _PTR_(a)              (volatile unsigned int *)(a)
#define _ARR_(a)              ((volatile unsigned int *)(a))
#define _PTRFV_(a)            (void (*)())(a)
#define _SELF_(t,m,n)         ((t*)((int)(&n) - (int)(&(((t*)0)->m))))

/* Registers control */
#define getreg32(a)          (*(volatile int *)(a))
#define putreg32(v,a)        (*(volatile int *)(a) = (v))

#include <utils/debug.h>

#endif /* __INCLUDE_UTIL_H */
