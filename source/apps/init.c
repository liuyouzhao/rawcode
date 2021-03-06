#include <unistd.h>
#include <arch.h>
#include <utils/util.h>

int rc_task_create(const char* name, void (*pfunc) (void* para), 
                            unsigned int stacksize, unsigned int prior, void *para);



void func4(int *dat)
{
    int *d = dat;
    *d = 256;
}
void func3(int *dat)
{
    int *d = dat;
    func4(d);
}


void func2(int *dat)
{
    int *d = dat;
    func3(d);
}

void func1(int *dat)
{
    int *d = dat;
    func2(d);
}



void task1(void* para)
{
    int i = 0;
    int j = 0;
    char *ptr = 0;
    int size = 0;
    //const char *a = "hello what's your name";
    printf("task1=====start\n");
    float y, x, a;
    while(1) {
        i ++;

        if(i > 99999) i = 0;

        size = ((i + 1) % 10) * 128;
        ptr = (char*) rc_malloc(size);
        rc_memset(ptr, 0, size);
        //printf("t1(%p)\n", ptr);
        printf("t1--(%p)\n", ptr);
        rc_free(ptr);
        printf("t1==(%p)\n", ptr);

        if(i > 1000) {
            printf("t1\n");
            i = 0;
        }

        printf("t1:--%d\n", i);
        func1(&j);
        printf("t1:%d\n", j);
    }
}
    

void task2(void* para)
{
    int i = 0;
    int j = 0;
    char *ptr = 0;
    int size = 0;
    float y, x, z,f;
    const char *c = ".:-=+*#%@";
    printf("task2=====start\n");
    while(1) {
        i ++;

        if(i > 99999) i = 0;
        size = ((i + 1) % 10) * 128;
        ptr = (char*) rc_malloc(size);
        //rc_memset(ptr, 0, size);
        printf("t2--(%p)\n", ptr);
        rc_free(ptr);
        printf("t2==(%p)\n", ptr);
        if(i > 1000) {
            printf("t2\n");
            i = 0;
        }
        printf("t2:-- %d\n", i);
        func1(&j);
        printf("t2:%d\n", j);
    }
}

void task3(void* para)
{
    int i = 0;
    int j = 0;
    char *ptr = 0;
    int size = 0;
    float y, x, z,f;
    const char *c = "0000000000";
    while(1) {
        i ++;

        if(i > 99999) i = 0;

        size = ((i + 1) % 10) * 128;
        ptr = (char*) rc_malloc(size);
        rc_memset(ptr, 0, size);
        printf("t3(%p)\n", ptr);
        rc_free(ptr);
        if(i > 1000) {
            printf("t3\n");
            i = 0;
        }

        printf("t3:-- %d\n", i);
        func1(&j);
        printf("t3:%d\n", j);
    }
}

int _query_code(int);
int _insert_code(int,int);
void switch_svc();
int rc_task_init();
int rc_mm_init();

int init()
{
    int ret;

    arch_init();
    rc_task_init();
    rc_mm_init();

    asm_close_irq();

    ret = rc_task_create("task1", task1, 4096, 10, NULL);
    ret |= rc_task_create("task2", task2, 4096, 10, NULL);
    ret |= rc_task_create("task3", task3, 4096, 10, NULL);

	kprintf("rc_task_create status: %d\n", ret);

    asm_open_irq();

    kprintf("finished init\n");

    int i = 0;
    for(;;) {}
    

    return 0;
}
