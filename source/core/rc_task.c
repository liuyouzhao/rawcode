#include <string.h>
#include <utils/util.h>
#include <utils/list.h>
#include <port/port.h>
#include <arch.h>

typedef struct rc_task_s
{
    rc_registers_t regs;

    unsigned int prio;

    arch_stack_type stack_low;
    unsigned int stack_size;

    char task_name[KC_TASK_MAX_NAME_LEN];

    void (*entry)(void* p);
    void *para;

    __L_NODE__
} rc_task_t;

typedef struct rc_tskstat_s
{
    unsigned int id;
    unsigned int stat;
    __L_NODE__
} rc_tskstat_t;

static list_t s_lst_tasks;
static list_t s_lst_ready;
static list_t s_lst_blcked;
static list_t s_lst_spded;

static rc_task_t *s_ptsk_running;
static unsigned char *s_cur_stack_ptr;

static void put_to_ready_list(rc_task_t *tsk);
void rc_task_switch();

static void rc_task_sys_tick()
{
    rc_task_switch();
}

int rc_task_init()
{
    s_cur_stack_ptr = g_pt->stack_low;

    s_lst_tasks.len = 0;
    s_lst_ready.len = 0;
    s_lst_blcked.len = 0;
    s_lst_spded.len = 0;

    arch_bind_systick(rc_task_sys_tick);
}

int rc_task_create(const char* name, void (*pfunc) (void* para), 
                            unsigned int stacksize, unsigned int prior, void *para)
{
    rc_task_t *tsk;

    if(s_lst_tasks.len >= KC_TASK_MAX_NUM || 
        s_cur_stack_ptr - stacksize < g_pt->stack_top) {
        __DEBUG__
        return -1;
    }

    tsk = (rc_task_t*) rc_malloc(sizeof(rc_task_t));

    if(tsk == NULL) {
        __DEBUG_ERR__("memory is not enough");
        return -1;
    }

    memcpy(tsk->task_name, name, strlen(name));
    tsk->prio = prior;
    tsk->para = para;
    tsk->stack_low = (unsigned int)s_cur_stack_ptr;
    tsk->stack_size = stacksize;
    tsk->entry = pfunc;

    g_pt->task_registers_init(&(tsk->regs));
    put_to_ready_list(tsk);
    return 0;
}

void rc_task_switch()
{
    l_node_t *pn;
    rc_task_t *last;

    g_pt->enter_critical();

    if(s_lst_ready.len == 0) {
        __DEBUG_ERR__("Ready length is 0");
        goto exit;
    }

    if(s_ptsk_running == NULL) {
        pn = list_pop_head(&s_lst_ready);
        s_ptsk_running = list_node_container(rc_task_t, *pn);
        g_pt->task_switch(&(s_ptsk_running->regs), NULL,
                            s_ptsk_running->stack_low, s_ptsk_running->stack_size);
        goto exit;
    }
    last = s_ptsk_running;
    put_to_ready_list(s_ptsk_running);

    pn = list_pop_head(&s_lst_ready);
    s_ptsk_running = list_node_container(rc_task_t, *pn);

    g_pt->task_switch(&(s_ptsk_running->regs), last,
                        s_ptsk_running->stack_low, s_ptsk_running->stack_size);
exit:
    g_pt->exit_critical();
}

static void put_to_ready_list(rc_task_t *tsk)
{
    l_node_t *p = s_lst_ready.p_head;
    rc_task_t *cur_tsk = NULL;
    while(p)
    {
        cur_tsk = list_node_container(rc_task_t, *p);
        if(cur_tsk->prio < tsk->prio) {
            break;
        }
        p = p->p_next;
    }
    if(cur_tsk == NULL) {
        list_add_tail(&s_lst_ready, tsk);
    }
    else {
        list_insert_ptr_next(&s_lst_ready, cur_tsk, tsk);

        p = s_lst_ready.p_head;
        while(p) {
            printf("ready_list: %p\n", p);
            p = p->p_next;
        }

    }
}

