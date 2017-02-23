#include <arch.h>
#include <utils/util.h>
#include <swi.h>

void (*swi_handler)();

void arch_bind_swi(void (*swi)())
{
    swi_handler = swi;
}

void software_interrupt(unsigned int inst, unsigned int sp)
{
    __KDEBUG__
    if(swi_handler != 0) {
        swi_handler();
    }
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
