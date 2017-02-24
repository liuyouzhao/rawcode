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

static int s_entered;
static unsigned long s_last_tick = 0;

/**
 * inner static functions
*/
static void put_to_ready_list(rc_task_t *tsk);
static void task_tick_switch();
static void rc_task_sys_tick();
static void rc_task_swi();

/*
 * Task API
*/
int rc_task_init()
{
    s_cur_stack_ptr = g_pt->stack_low;

    s_lst_tasks.len = 0;
    s_lst_ready.len = 0;
    s_lst_blcked.len = 0;
    s_lst_spded.len = 0;

    s_entered = 0;

    g_pt->arch_bind_systick(rc_task_sys_tick);
    g_pt->arch_bind_swi(rc_task_swi);
}

int rc_task_create(const char* name, void (*pfunc) (void* para), 
                            unsigned int stacksize, unsigned int prior, void *para)
{
    rc_task_t *tsk;

    g_pt->enter_critical();

    if(s_lst_tasks.len >= KC_TASK_MAX_NUM || 
        s_cur_stack_ptr - stacksize < g_pt->stack_top) {
        __DEBUG__
        return -1;
    }

    tsk = (rc_task_t*) rc_kmalloc(sizeof(rc_task_t));

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

    g_pt->task_registers_init(&(tsk->regs), tsk->entry, tsk->para, tsk->stack_low);
    put_to_ready_list(tsk);

    s_cur_stack_ptr -= stacksize;

    g_pt->exit_critical();

    return 0;
}

void rc_task_enter_section()
{
    s_entered ++;
    s_last_tick = g_pt->global_tick;
}

void rc_task_exit_section()
{
    s_entered --;
    if(s_entered < 0) s_entered = 0;
}

void rc_task_clear_section()
{
    s_entered = 0;
}

void rc_task_interrupt()
{
    g_pt->enter_critical();
    rc_task_clear_section();
    g_pt->task_interrupt();
}

void rc_task_try_switch()
{
    l_node_t *pn;
    rc_task_t *last;

    if(s_last_tick < g_pt->global_tick) {
        g_pt->enter_critical();
        rc_task_clear_section();
        
        if(s_lst_ready.len == 0) {
            g_pt->exit_critical();
            return;
        }

        last = s_ptsk_running;
        put_to_ready_list(s_ptsk_running);

        pn = list_pop_head(&s_lst_ready);
        s_ptsk_running = list_node_container(rc_task_t, *pn);

#if 0
        if(s_ptsk_running->regs.regs[13] == 0x0) {

            s_ptsk_running->regs.regs[13] = s_ptsk_running->stack_low;
            s_ptsk_running->regs.regs[0] = (unsigned int)s_ptsk_running->para;
        }
#endif

        asm_task_save_switch(last->regs.regs, s_ptsk_running->regs.regs);
    }
}

static void rc_task_sys_tick()
{
    if(s_entered) {
        return;
    }
    task_tick_switch();
}

static void rc_task_swi()
{
    if(s_entered) {
        return;
    }
    task_tick_switch();
}

static void task_tick_switch()
{
    l_node_t *pn;
    rc_task_t *last;

    g_pt->enter_critical();

    if(s_lst_ready.len == 0) {
        if(s_ptsk_running != NULL) {
             g_pt->task_switch(&(s_ptsk_running->regs), NULL,
                            s_ptsk_running->stack_low, s_ptsk_running->stack_size,
                            s_ptsk_running->para);
        }
        goto exit;
    }

    if(s_ptsk_running == NULL) {
        pn = list_pop_head(&s_lst_ready);
        s_ptsk_running = list_node_container(rc_task_t, *pn);
        g_pt->task_switch(&(s_ptsk_running->regs), NULL,
                            s_ptsk_running->stack_low, s_ptsk_running->stack_size,
                            s_ptsk_running->para);
        goto exit;
    }
    last = s_ptsk_running;
    put_to_ready_list(s_ptsk_running);

    pn = list_pop_head(&s_lst_ready);
    s_ptsk_running = list_node_container(rc_task_t, *pn);

    g_pt->task_switch(&(s_ptsk_running->regs), last,
                        s_ptsk_running->stack_low, s_ptsk_running->stack_size,
                        s_ptsk_running->para);
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
            p = p->p_next;
        }

    }
}

