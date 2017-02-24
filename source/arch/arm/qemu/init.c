#include <unistd.h>
#include <arch.h>
#include <utils/util.h>

int rc_task_create(const char* name, void (*pfunc) (void* para), 
                            unsigned int stacksize, unsigned int prior, void *para);



void task1(void* para)
{
    int i = 0;
    int j = 0;
    char *ptr = 0;
    int size = 0;
    const char *a = "hello what's your name";
    printf("task1=====start\n");
    while(1) {
        i ++;
        printf("task1:%d  %s  sp: %p a: %p %p %p\n", i, a, asm_get_sp(), &a, &i, &j);


        float y, x, a;
	    for (y = 1.5f;y > -1.5f;y -= 0.1f)
	    {
		    for (x = -1.5f;x < 1.5f;x += 0.05f)
		    {
			    a = x*x + y*y - 1;
			    printf(a*a*a - x*x*y*y*y <= 0.0f ? "*" : " ");
		    }
		    printf("\n");
	    }

#if 1
        size = ((i + 1) % 10) * 128;
        ptr = (char*) rc_malloc(size);
        rc_memset(ptr, 0, size);
        rc_free(ptr);
#endif
        //for(j = 0; j < 1000000; j ++);
        //printf("task1[%d] \n", i ++);
    }
}
    

void task2(void* para)
{
    int i = 0;
    int j = 0;
    char *ptr = 0;
    int size = 0;

    while(1) {
        i ++;
        printf("task2:%d  %p\n", i, asm_get_sp());

        float y, x, z,f;
        const char *c = ".:-=+*#%@";
	    for (y = 1.5f;y > -1.5f;y -= 0.1f)
	    {
		    for (x = -1.5f;x < 1.5f;x += 0.05f)
		    {
			    z = x*x + y*y - 1;
			    f = z*z*z - x*x*y*y*y;
			    printf("%c", f <= 0.0f ? c[(int)(f*-8.0f)] : ' ');
		    }
		    printf("\n");
	    }
#if 1
        size = ((i + 1) % 10) * 128;
        ptr = (char*) rc_malloc(size);
        rc_memset(ptr, 0, size);
        rc_free(ptr);
#endif
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

    ret = rc_task_create("task1", task1, 4096, 10, NULL);
    __KDEBUG__
    ret |= rc_task_create("task2", task2, 4096, 10, NULL);

    kprintf("finished init\n");

    int i = 0;
    for(;;) {}
    

    return 0;
}
