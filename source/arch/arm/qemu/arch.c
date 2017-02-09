#include <arch.h>
#include <utils/util.h>
#include <port/port.h>

/************************************
* Global address pointers definition
*************************************/
unsigned char* glb_output_uart_addr = (unsigned char*) IC_UART0;
static port_t s_arch_port;

/********************************
 * Register base functions
********************************/


/********************************
 * Port
********************************/
static void _arch_port_reset()
{
}

static void _arch_port_panic()
{
    __DEBUG__
    printf("Kernel Panic!\n");

    asm_close_irq();
    while(1);
}

static void _arch_enter_critical()
{
    asm_close_irq();
    s_arch_port.critical_nesting ++;
}

static void _arch_exit_critical()
{
    s_arch_port.critical_nesting --;
    if(s_arch_port.critical_nesting < 0) {
        s_arch_port.critical_nesting = 0;
    }

    if(s_arch_port.critical_nesting == 0) {
        asm_open_irq();
    }
}

static void _arch_task_registers_init(void *registers)
{
    int i;
    for( i = 0; i < 18; i ++ )
        ((rc_registers_t*)registers)->regs[i] = 0;
}

static void _arch_task_switch(void *regs, void *last_regs,
                                unsigned int stack_low, unsigned int stack_size)
{
    rc_registers_t *rgs = regs;
    rc_registers_t *lst_rgs = last_regs;
    int i;

    if(lst_rgs) {
        printf("BEFORE: \n");
        for(i = 0; i < 18; i ++) {
            printf("r%d: %x ", i, lst_rgs->regs[i]);
            if((i + 1) % 4 == 0) {
                printf("\n");
            }
        }
        printf("\n");
        asm_task_save_context(lst_rgs->regs);

        printf("AFTER: \n");
        for(i = 0; i < 18; i ++) {
            printf("r%d: %x ", i, lst_rgs->regs[i]);
            if((i + 1) % 4 == 0) {
                printf("\n");
            }
        }
        printf("\n");
    }
    else {
        printf("There's only one task\n");
    }
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
    .stack_top = (unsigned char*) KC_TASK_STACK_TOP,
    .task_registers_init = _arch_task_registers_init,
    .task_switch = _arch_task_switch,

    /* Memory */
    .heap_low = (unsigned char*) KC_MEM_HEAP_LOW,
    .heap_top = (unsigned char*) KC_MEM_HEAP_TOP,
    .slab_unit = KC_MEM_SLAB_UNIT,
    .slab_nmax = KC_MEM_SLAB_NMAX,
    .prsv_chunk = KC_MEM_PRESERVE_CHUNK,
};

void arch_init()
{
    arch_tick_init();
    g_pt = &s_arch_port;
}
