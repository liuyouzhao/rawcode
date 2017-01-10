#include <stdio.h>


typedef struct
{
    void* head;
    unsigned int len;
} rc_list_s;

typedef struct
{
    void* p_next;
} rc_node_s;

int add_tail(rc_list_s *list, rc_node_s *node)
{
    if(list == NULL)
    {}
}
