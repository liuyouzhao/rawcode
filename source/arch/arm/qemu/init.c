#include <unistd.h>
#include <arch.h>
#include <utils/util.h>

int rc_task_create(const char* name, void (*pfunc) (void* para), 
                            unsigned int stacksize, unsigned int prior, void *para);


void task1(void* para)
{
    printf("task1\n");
}

void task2(void* para)
{
    printf("task2\n");
}


int init()
{
    int begin = 0x0;
    int cmd = 0x0;
    int ret;
    
    for(begin = 0x0; begin < 0x80; begin += 0x4)
    {
        cmd = _query_code(0x10000 + begin);
        _insert_code(cmd, begin);
        printf("%x\n", _query_code(begin));
    }



    switch_svc();

    /**
     * Problems:
     * here cannot open either MPU nor MMU
     */
    //asm_mpu_config();


    arch_init();
    rc_task_init();
    rc_mm_init();

    /* test go */
    test_mem();


    ret = rc_task_create("task1", task1, 4096, 10, NULL);
    ret |= rc_task_create("task2", task2, 4096, 10, NULL);

    for(;;) {}

    return 0;
}
