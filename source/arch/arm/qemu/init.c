#include <unistd.h>
#include <arch.h>
#include <utils/util.h>

int rc_task_create(const char* name, void (*pfunc) (void* para), 
                            unsigned int stacksize, unsigned int prior, void *para);


void task1(void* para)
{
    int i = 9;
    int j = 0;
    const char* a = "hello what's your name";
    printf("task1=====start\n");
    while(1) {
        i ++;
        printf("task1:%d  %s  sp: %p a: %p %p %p\n", i, a, asm_get_sp(), &a, &i, &j);
        for(j = 0; j < 1000000; j ++);
        //printf("task1[%d] \n", i ++);
    }
}
    

void task2(void* para)
{
    int i = 0;
    int j = 0;
    while(1) {
        i ++;
        printf("task2:%d  %p\n", i, asm_get_sp());
        for(j = 0; j < 1000000; j ++);
        //printf("task2[%d] \n", i ++);
    }
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
        //printf("%x\n", _query_code(begin));
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
    //test_mem();


    ret = rc_task_create("task1", task1, 4096, 10, NULL);
    ret |= rc_task_create("task2", task2, 4096, 10, NULL);

    int i = 0;
    for(;;) {
        //printf("i: %d\n", i++);
    }

    return 0;
}
