#include <stdio.h>
#include <utils/list.h>
#include <port/port.h>

typedef struct rc_queue_s
{
    list_t qlst;
} rc_queue_t;

typedef struct rc_node_s
{
    __L_NODE__(qlst)
} rc_node_t;


rc_queue_t* rc_queue_create()
{
}

int rc_queue_create_mutex()
{
}

int rc_queue_add_tail(rc_queue_t *queue, rc_node_t *node)
{
}

int rc_queue_recv()
{
    g_pt->enter_critical();
    g_pt->exit_critical();
}

int rc_queue_send()
{
}
