/********************************************************************************
 * utils/list.h
 *
 *   Copyright (C) 2016,2017 Frodo Liu. All rights reserved.
 *   Author: Frodo Liu <liuyouzhao@hotmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name RawCode nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ********************************************************************************/
#ifndef __INCLUDE_LIST_H
#define __INCLUDE_LIST_H

typedef struct l_node_s
{
    struct l_node_s *p_prev;
    struct l_node_s *p_next;
}l_node_t;

typedef struct list_s
{
    l_node_t *p_head;
    l_node_t *p_tail;

    unsigned int len;

}list_t;

static inline int __l_add_tail(list_t *lst, l_node_t *node)
{
    if(lst->len == 0) {
        lst->p_head = lst->p_tail = node;
    }
    else {
        lst->p_tail->p_next = node;
        node->p_prev = lst->p_tail;
    }
    lst->len ++;
    return lst->len;
}

static inline int __l_add_head(list_t *lst, l_node_t *node)
{
    if(lst->len == 0) {
        lst->p_head = lst->p_tail = node;
    }
    else {
        lst->p_head->p_prev = node;
        node->p_next = lst->p_head;
    }
    lst->len ++;
    return lst->len;
}

static inline int __l_insert_ptr_next(list_t *lst, l_node_t *whr, l_node_t *node)
{
    if(whr == lst->p_tail) {
        __l_add_tail(lst, node);
        return lst->len;
    }

    whr->p_next->p_prev = node;
    node->p_next = whr->p_next;
    whr->p_next = node;
    node->p_prev = whr;
    lst->len ++;
    return lst->len;
}

static inline int __l_insert_ptr_prev(list_t *lst, l_node_t *whr, l_node_t *node)
{
    if(whr == lst->p_head) {
        __l_add_head(lst, node);
        return lst->len;
    }

    whr->p_prev->p_next = node;
    node->p_next = whr;
    node->p_prev = whr->p_prev;
    whr->p_prev = node;
    lst->len ++;
    return lst->len;
}

static inline int __l_insert_index_next(list_t *lst, unsigned int index, l_node_t *node)
{
    unsigned int i = 0;
    l_node_t *p = lst->p_head;

    for( ; i < index; i ++ ) {
        p = p->p_next;
    }
    __l_insert_ptr_next(lst, p, node);

    return lst->len;
}

static inline int __l_insert_index_prev(list_t *lst, unsigned int index, l_node_t *node)
{
    unsigned int i = 0;
    l_node_t *p = lst->p_head;

    for( ; i < index; i ++ ) {
        p = p->p_next;
    }
    __l_insert_ptr_prev(lst, p, node);

    return lst->len;
}

static inline int __l_del_head(list_t *lst)
{
    lst->p_head = lst->p_head->p_next;
    lst->p_head->p_prev = 0;
    lst->len --;
    return lst->len;
}

static inline int __l_del_tail(list_t *lst)
{
    lst->p_tail = lst->p_tail->p_prev;
    lst->p_tail->p_next = 0;
    lst->len --;
    return lst->len;
}

static inline l_node_t * __l_pop_head(list_t *lst)
{
    l_node_t *rt = lst->p_head;
    __l_del_head(lst);
    return rt;
}

static inline l_node_t * __l_pop_tail(list_t *lst)
{
    l_node_t *rt = lst->p_tail;
    __l_del_tail(lst);
    return rt;
}

static inline l_node_t * __l_next(l_node_t **from)
{
    *from = (*from)->p_next;
    return *from;
}

#define __L_NODE_NAME __l_node_obj_160624065_
#define __L_NODE__ l_node_t __L_NODE_NAME;
#define list_add_tail(l,s) \
    __l_add_tail(l, &(s->__L_NODE_NAME))
#define list_add_head(l,s) \
    __l_add_tail(l, &(s->__L_NODE_NAME))
#define list_insert_ptr_next(l,p,s) \
    __l_insert_ptr_next(l, &(p->__L_NODE_NAME), &(s->__L_NODE_NAME))
#define list_insert_ptr_prev(l,p,s) \
    __l_insert_ptr_prev(l, &(p->__L_NODE_NAME), &(s->__L_NODE_NAME))
#define list_insert_index_next(l,i,s) \
    __l_insert_index_next(l, i, s->__L_NODE_NAME)
#define list_insert_index_prev(l,i,s) \
    __l_insert_index_prev(l, i, s->__L_NODE_NAME)
#define list_del_head(l) \
    __l_del_head(l)
#define list_del_tail(l) \
    __l_del_tail(l)
#define list_pop_head(l) \
    __l_pop_head(l)
#define list_pop_tail(l) \
    __l_pop_tail(l)
#define list_next(f) \
    __l_next(&&f)

#define list_node_container(t,n) \
    _SELF_(t,__L_NODE_NAME,n)

#define l_p_n(c) \
    c->__L_NODE_NAME

#define l_n(c) \
    c.__L_NODE_NAME


#endif /* __INCLUDE_LIST_H */
