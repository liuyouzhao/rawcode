#include <utils/util.h>
#include <utils/list.h>
#include <arch.h>

typedef struct rc_task_s
{
    rc_registers_t regs;

    unsigned int prio;

    arch_stack_type *p_stack;

    char task_name[KC_TASK_MAX_NAME_LEN];

    __L_NODE__(tsk_stat)

} rc_task_t;

static rc_task_t s_tasks[KC_TASK_MAX_NUM];

static void rc_task_sys_tick()
{
    __DEBUG__
}

int rc_task_init()
{
    arch_bind_systick(rc_task_sys_tick);
}
