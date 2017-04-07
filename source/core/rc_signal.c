#include <stdio.h>
#include <utils/list.h>
#include <utils/debug.h>
#include <port/port.h>
#include <arch.h>

typedef struct rc_signal_lstnr_s
{
    void *tsk;
    __L_NODE__
} rc_signal_lstnr_t;

typedef struct rc_signal_bndle_s
{
    list_t lstnrs;
    int idle;
} rc_signal_bndle_t;

typedef unsigned int rc_signal_t;

static unsigned int s_idle = 0;
static rc_signal_bndle_t *s_bundles[KC_SIGNAL_MAX];

list_t rc_signal_init()
{
    int i = 0;
    g_pt->enter_critical();

    for( ; i < KC_SIGNAL_MAX; i ++ ) {
        s_bundles[i] = (rc_signal_bndle_t*) rc_kmalloc(sizeof(rc_signal_bndle_t));
        s_bundles[i]->idle = 1;
    }
    s_idle = 0;

    g_pt->exit_critical();
}

void rc_signal_create(rc_signal_t *signal)
{
    g_pt->enter_critical();

    if(s_idle >= KC_SIGNAL_MAX) {
        __KDEBUG_ERR__("signal number overflow");
        goto exit;
    }
    *signal = s_idle;

    while(!s_bundles[s_idle]->idle && s_idle < KC_SIGNAL_MAX) {
        s_idle ++;
    }
exit:
    g_pt->exit_critical();
    return;
}

int rc_signal_recv(rc_signal_t signal, unsigned int timeout)
{
    rc_signal_lstnr_t *lstnr = NULL;

    g_pt->enter_critical();

    lstnr = (rc_signal_lstnr_t*) rc_kmalloc(sizeof(rc_signal_lstnr_t));
    lstnr->tsk = (void*) rc_task_running();

    if(s_bundles[signal]->idle) {
        s_bundles[signal]->idle = 0;
    }

    list_add_tail(&s_bundles[signal]->lstnrs, lstnr);

    rc_task_suspend();
    rc_task_switch();
}

int rc_signal_post()
{
}
