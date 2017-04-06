#include <stdio.h>
#include <utils/list.h>
#include <port/port.h>


typedef struct rc_node_s
{
    __L_NODE__
} rc_node_t;

static list_t s_qlst;

list_t* rc_queue_create()
{
}

int rc_queue_create_mutex()
{
}

int rc_queue_add_tail(list_t qlst, rc_node_t *node)
{
}

int rc_queue_recv()
{
    g_pt->enter_critical();
    g_pt->exit_critical();
}

int rc_queue_post()
{
}
