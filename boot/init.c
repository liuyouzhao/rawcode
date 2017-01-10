#include <unistd.h>
#include <config.h>
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

    boot_timer_init();

    for(;;) {}
    //tick_loop(uart0_printf);

    return 0;
}
