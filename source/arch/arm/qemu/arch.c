#include <arch.h>
#include <utils/util.h>
#include <port/port.h>
#include "irq.h"
#include "swi.h"

/************************************
* Global address pointers definition
*************************************/
unsigned char* glb_output_uart_addr = (unsigned char*) IC_UART0;
static int s_closed_irq = 0;
extern unsigned long timer_tick;

/********************************
 * Register base functions
#if 1
    kprintf("\nSWITCH: \n");
    for(i = 0; i < 18; i ++) {
        kprintf("r%d: %x ", i, rgs->regs[i]);
        if((i + 1) % 4 == 0) {
            kprintf("\n");
        }
    }
    kprintf("\n");
#endif
********************************/


/********************************
 * Port
********************************/
static void _arch_port_reset()
{
}

static void _arch_port_panic()
{
    __KDEBUG_ERR__("Kernel Panic!\n");
    asm_close_irq();
    while(1);
}

static void _arch_enter_critical()
{
    asm_close_irq();
    g_pt->critical_nesting ++;
}

static void _arch_exit_critical()
{
    if( g_pt->critical_nesting == 0 ) {
        asm_open_irq();
    }
    g_pt->critical_nesting --;
}

static void _arch_task_registers_init(void *registers, void *entry, void *para, unsigned int stack_low)
{
    int i;
    for( i = 0; i < 18; i ++ )
        ((rc_registers_t*)registers)->regs[i] = 0;

    ((rc_registers_t*)registers)->regs[0] = (unsigned int) para;
    ((rc_registers_t*)registers)->regs[13] = stack_low;
    ((rc_registers_t*)registers)->regs[14] = (unsigned int) entry;
    ((rc_registers_t*)registers)->regs[15] = (unsigned int) entry;
}

static void _arch_task_switch(void *regs, void *last_regs,
                                unsigned int stack_low, unsigned int stack_size, void* para)
{
    rc_registers_t *rgs = regs;
    rc_registers_t *lst_rgs = last_regs;
    int i;

    arch_tick_disable();

    if(lst_rgs) {
        unsigned int *pr = (unsigned int*)(0x14000);

        kprintf("save context \n");
        asm_task_save_context(lst_rgs->regs);

        kprintf("\n");
    }

    arch_tick_enable();
    asm_task_switch_context(rgs->regs);
}

void _arch_task_enter_swi() {
    asm_task_enter_swi();
}

static port_t s_arch_port =
{
    .reset = _arch_port_reset,
    .panic = _arch_port_panic,

    .enter_critical = _arch_enter_critical,
    .exit_critical = _arch_exit_critical,
    .critical_nesting = 0,

    .uart0_addr = (unsigned char*) IC_UART0,
    .uart1_addr = (unsigned char*) IC_UART1,

    /* Tasks */
    .stack_low = (unsigned char*) KC_TASK_STACK_LOW,
    .stack_top = (unsigned char*) KC_TASK_STACK_TOP,
    .task_registers_init = _arch_task_registers_init,
    .task_switch = _arch_task_switch,
    .task_interrupt = _arch_task_enter_swi,

    .arch_bind_systick = arch_bind_systick,
    .arch_bind_swi = arch_bind_swi,

    /* Memory */
    .heap_low = (unsigned char*) KC_MEM_HEAP_LOW,
    .heap_top = (unsigned char*) KC_MEM_HEAP_TOP,
    .slab_unit = KC_MEM_SLAB_UNIT,
    .slab_nmax = KC_MEM_SLAB_NMAX,
    .prsv_chunk = KC_MEM_PRESERVE_CHUNK,

    /* Global tick */
    .global_tick = 0
};

void arch_init()
{
    arch_tick_init();
    g_pt = &s_arch_port;
}

void __dbg__() {
    static int i = 0;
    kprintf("dgb %d\n", i++);
}
