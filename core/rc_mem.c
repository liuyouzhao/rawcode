#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
    unsigned int stack_heap_top;
    unsigned int stack_heap_bottom;
    
} rc_heap_s;


void *malloc(size_t size)
{
    return 0;
}


void free(void *ptr)
{

}



#if 0
#define MEM_SIZE_WARNING 30*1024
#define MEM_SIZE_BEFORE_DIE 48*1024
#define __MEM_IF_INIT__ if (g_inited == 0) { \
                            csp_sem_new(&g_sem, 0); \
                            list_init(&g_yoc_debug_mem_list); g_inited = 1;}

#define __MEM_LOCK__   csp_sem_wait(g_sem, 1);
#define __MEM_UNLOCK__ csp_sem_signal(g_sem);

struct mem_node {
    struct list _list_node;
    void *_ptr_addr;
    unsigned int _size;
    char _file[32];
    unsigned int _line;
};

struct list g_yoc_debug_mem_list;
static unsigned int g_pt_num = 0;
static unsigned int g_inited = 0;
static unsigned int g_mem_size = 0;
static csp_sem_t g_sem;

void _debug_memdump(void);


char *_debug_strdup(const char *s, char *file, unsigned int line)
{
    char *str = 0;
    struct mem_node *_m = (struct mem_node *)malloc(sizeof(struct mem_node));
    memset(_m, 0, sizeof(struct mem_node));

    __MEM_IF_INIT__

    if (g_mem_size >= MEM_SIZE_WARNING) {
        printf("[MEMORY DEBUG WARNING: pointers number: %d memory size: %d]\n", g_pt_num, g_mem_size);
        _debug_memdump();
    } else if (g_mem_size >= MEM_SIZE_BEFORE_DIE) {
        printf("[MEMORY DEBUG NEARLY DIED: pointers number: %d memory size: %d]\n", g_pt_num, g_mem_size);
        _debug_memdump();
    }

    str = strdup(s);

    if (str) {
        __MEM_LOCK__

        _m->_ptr_addr = str;
        _m->_size = strlen(str + 1);
        memcpy(_m->_file, file, strlen(file));
        _m->_line = line;

        list_init(&(_m->_list_node));
        list_add(&(_m->_list_node), &g_yoc_debug_mem_list);

        g_pt_num ++;
        g_mem_size += _m->_size;

        __MEM_UNLOCK__
    }

    return str;
}

void *_debug_malloc(unsigned int num_bytes, char *file, unsigned int line)
{
    struct mem_node *_m = (struct mem_node *)malloc(sizeof(struct mem_node));
    memset(_m, 0, sizeof(struct mem_node));

    __MEM_IF_INIT__

    if (g_mem_size >= MEM_SIZE_WARNING) {
        printf("[MEMORY DEBUG WARNING: pointers number: %d memory size: %d]\n", g_pt_num, g_mem_size);
        _debug_memdump();
    } else if (g_mem_size >= MEM_SIZE_BEFORE_DIE) {
        printf("[MEMORY DEBUG NEARLY DIED: pointers number: %d memory size: %d]\n", g_pt_num, g_mem_size);
        _debug_memdump();
    }

    void *p = malloc(num_bytes);

    if (p) {
        __MEM_LOCK__

        _m->_ptr_addr = p;
        _m->_size = num_bytes;
        memcpy(_m->_file, file, strlen(file));
        _m->_line = line;

        list_init(&(_m->_list_node));
        list_add(&(_m->_list_node), &g_yoc_debug_mem_list);

        g_pt_num ++;
        g_mem_size += num_bytes;

        __MEM_UNLOCK__
    }

    return p;
}

void *_debug_calloc(size_t n, size_t size, char *file, unsigned int line)
{
    struct mem_node *_m = (struct mem_node *)malloc(sizeof(struct mem_node));
    memset(_m, 0, sizeof(struct mem_node));

    __MEM_IF_INIT__

    if (g_mem_size >= MEM_SIZE_WARNING) {
        printf("[MEMORY DEBUG WARNING: pointers number: %d memory size: %d]\n", g_pt_num, g_mem_size);
        _debug_memdump();
    } else if (g_mem_size >= MEM_SIZE_BEFORE_DIE) {
        printf("[MEMORY DEBUG NEARLY DIED: pointers number: %d memory size: %d]\n", g_pt_num, g_mem_size);
        _debug_memdump();
    }

    void *p = calloc(n, size);

    if (p) {
        __MEM_LOCK__
        _m->_ptr_addr = p;
        _m->_size = n * size;
        memcpy(_m->_file, file, strlen(file));
        _m->_line = line;

        list_init(&(_m->_list_node));
        list_add(&(_m->_list_node), &g_yoc_debug_mem_list);

        g_pt_num ++;
        g_mem_size += _m->_size;

        __MEM_UNLOCK__

    }

    return p;
}

void _debug_free(void *ptr)
{
    struct list *pos = 0;
    struct mem_node *pm = 0;

    __MEM_LOCK__
    list_for_each(pos, &g_yoc_debug_mem_list) {
        pm = list_entry(pos, struct mem_node, _list_node);

        if (pm->_ptr_addr == ptr) {
            g_mem_size -= pm->_size;
            g_pt_num --;
            list_del(pos);
            free(pm);
            break;
        }
    }
    __MEM_UNLOCK__

    free(ptr);
}

void *_debug_realloc(void *ptr, unsigned int num_bytes, char *file, unsigned int line)
{
    void *temp = NULL;

    if (num_bytes <= 0)
    { return NULL; }

    if (NULL == (temp = _debug_malloc(num_bytes, file, line))) {
        return NULL;
    }

    if (ptr) {
        memcpy(temp, ptr, num_bytes);
        _debug_free(ptr);
    }

    return temp;
}

void _debug_memdump()
{
#ifdef YOC_DEBUG_MEMORY_DUMP
    struct list *pos = 0;
    struct mem_node *pm = 0;

    printf("[MEMORY DEBUG INFO: pointers number: %d memory size: %d]\n", g_pt_num, g_mem_size);

    if (!g_pt_num || !g_mem_size)
    { return; }

    __MEM_LOCK__
    list_for_each(pos, &g_yoc_debug_mem_list) {
        pm = list_entry(pos, struct mem_node, _list_node);
        printf("[%s:%d %p:%d]\n", pm->_file, pm->_line, pm->_ptr_addr, pm->_size);
    }
    __MEM_UNLOCK__
#endif
}

void _debug_mem_usage(void)
{
    printf("[MEMORY DEBUG INFO: pointers number: %d memory size: %d]\n", g_pt_num, g_mem_size);
}

int _debug_mem_used_size(void)
{
    return g_mem_size;
}

int _debug_mem_used_pointers(void)
{
    return g_pt_num;
}
#endif
