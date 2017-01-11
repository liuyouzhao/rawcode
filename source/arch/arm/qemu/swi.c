#include <arch.h>

void software_interrupt(unsigned int inst, unsigned int sp)
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
