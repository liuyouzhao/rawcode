#ifndef __INCLUDE_ARCH_H
#define __INCLUDE_ARCH_H

/**
 * Interrupt configs
*/
/* Memory map for qemu Versatile/PB:  */
#define IC_SYSTEM_REGISTERS                0x10000000
#define IC_PCI_CONTROLLER_CONFIG_REGISTERS 0x10001000
#define IC_SERIAL_BUS_INTERFACE            0x10002000
#define IC_SECONDARY_INTTERUPT_CONTROLLER  0x10003000
#define IC_AACI                            0x10004000
#define IC_MMCI0                           0x10005000
#define IC_KMI0                            0x10006000
#define IC_KMI1                            0x10007000
#define IC_CLCD                            0x10008000
/*  0x10140000 Vectored interrupt controller.  */
#define IC_VIC_BASE                        0x10140000
#define IC_SYS_CTRL                        0x101e0000
#define IC_WATCHDOG                        0x101e1000
#define IC_TIMER0                          0x101e2000
#define IC_TIMER1                          0x101e3000
#define IC_GPIO0                           0x101e4000
#define IC_GPIO1                           0x101e5000
#define IC_GPIO2                           0x101e6000
#define IC_GPIO3                           0x101e7000
#define IC_RTC                             0x101e8000            
#define IC_UART0                           0x101f1000
#define IC_UART1                           0x101f2000
#define IC_UART2                           0x101f3000


/**
 * Kernel configs
*/
/* task */
#define KC_TASK_MAX_NUM                    32
#define KC_TASK_TOP                        0x00800000
#define KC_TASK_STACK_MINSIZE              0xff
#define KC_TASK_MAX_NAME_LEN               64
#define KC_TASK_STACK_TOP                  0x17000
#define KC_TASK_STACK_LOW                  KC_TASK_STACK_TOP + 0x8000

/* memory */
#define KC_MEM_HEAP_LOW                    0x1f000
#define KC_MEM_HEAP_TOP                    0x2f000
#define KC_MEM_SLAB_UNIT                   64
#define KC_MEM_SLAB_NMAX                   512
#define KC_MEM_PRESERVE_CHUNK              3

/* signal */
#define KC_SIGNAL_OFFSET                   1
#define KC_SIGNAL_MAX                      32

/* timer */
/* Max number timers, indeed the size that avoid to be too large.
   Otherwise function "rc_timer_create" cannot warrant the instantaneity that in real-time.
*/
#define KC_TIMER_MAX_NUM                   64

typedef struct arch_registers_s
{
    unsigned int regs[18];
} arch_registers_t;

/* Base types */
typedef unsigned int arch_stack_type;

typedef arch_registers_t rc_registers_t;

void arch_init();

#endif /* __INCLUDE_ARCH_H */

/* Memory map for Versatile/PB:  */
/* 0x10000000 System registers.  */
/* 0x10001000 PCI controller config registers.  */
/* 0x10002000 Serial bus interface.  */
/*  0x10003000 Secondary interrupt controller.  */
/* 0x10004000 AACI (audio).  */
/*  0x10005000 MMCI0.  */
/*  0x10006000 KMI0 (keyboard).  */
/*  0x10007000 KMI1 (mouse).  */
/* 0x10008000 Character LCD Interface.  */
/*  0x10009000 UART3.  */
/* 0x1000a000 Smart card 1.  */
/*  0x1000b000 MMCI1.  */
/*  0x10010000 Ethernet.  */
/* 0x10020000 USB.  */
/* 0x10100000 SSMC.  */
/* 0x10110000 MPMC.  */
/*  0x10120000 CLCD Controller.  */
/*  0x10130000 DMA Controller.  */
/*  0x10140000 Vectored interrupt controller.  */
/* 0x101d0000 AHB Monitor Interface.  */
/* 0x101e0000 System Controller.  */
/* 0x101e1000 Watchdog Interface.  */
/* 0x101e2000 Timer 0/1.  */
/* 0x101e3000 Timer 2/3.  */
/* 0x101e4000 GPIO port 0.  */
/* 0x101e5000 GPIO port 1.  */
/* 0x101e6000 GPIO port 2.  */
/* 0x101e7000 GPIO port 3.  */
/* 0x101e8000 RTC.  */
/* 0x101f0000 Smart card 0.  */
/*  0x101f1000 UART0.  */
/*  0x101f2000 UART1.  */
/*  0x101f3000 UART2.  */
/* 0x101f4000 SSPI.  */
/* 0x34000000 NOR Flash */
