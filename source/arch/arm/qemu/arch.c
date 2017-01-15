#include <arch.h>
#include <utils/util.h>
#include <port/port.h>

/************************************
* Global address pointers definition
*************************************/
unsigned char* glb_output_uart_addr = (unsigned char*) IC_UART0;


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
}

static port_t s_arch_port =
{
    .reset = _arch_port_reset,
    .panic = _arch_port_panic,

    .uart0_addr = (unsigned char*) IC_UART0,
    .uart1_addr = (unsigned char*) IC_UART1,

    .heap_low = (unsigned char*) KC_MEM_HEAP_LOW,
    .heap_top = (unsigned char*) KC_MEM_HEAP_TOP,
    .slab_unit = KC_MEM_SLAB_NMAX,
    .slab_nmax = KC_MEM_SLAB_UNIT,
    .prsv_chunk = KC_MEM_PRESERVE_CHUNK
};

void arch_init()
{
    arch_tick_init();
    g_pt = &s_arch_port;
}
