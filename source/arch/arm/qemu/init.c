#include <unistd.h>
#include <arch.h>
#include <utils/util.h>

int init()
{
    int begin = 0x0;
    int cmd = 0x0;
    
    for(begin = 0x0; begin < 0x80; begin += 0x4)
    {
        cmd = _query_code(0x10000 + begin);
        _insert_code(cmd, begin);
        printf("%x\n", _query_code(begin));
    }

    arch_init();

    rc_task_init();

    for(;;) {
//        printf("*");
    }

    return 0;
}