#include <config.h>
#include <utils/util.h>

void undefined_instruction()
{ 
    __DEBUG__
}

void software_interrupt(unsigned int inst, unsigned int sp)
{
    __DEBUG__
}

void prefetch_abort()
{
    __DEBUG__
}

void data_abort()
{
    __DEBUG__
}

void not_used()
{
    __DEBUG__
}

void irq()
{
    printf("VIC: [%x] \n", getreg32(CONF_VIC_BASE));
    boot_tick_done();
    __DEBUG__
}

void fiq()
{
    __DEBUG__
}

/*************************************************
Backup code for reference:
    printf("[software_interrupt]\n");
    printhex32(inst);
    printhex32(sp);
    printhex32(_query_code(sp + 0x0));
    printhex32(_query_code(sp + 0x4));
    printhex32(_query_code(sp + 0x8));
    printf("[software_interrupt end]\n");
*/
