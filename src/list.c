

#include "list.h"

int m_list_init(struct m_list *list, size_t offset)
{
    if (!list) return M_EINVAL;

    list->head = list->tail = NULL;
    list->offset = offset;
    list->length = 0;

    return 0;
}

int m_list_free(struct m_list *list,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!list) return M_EINVAL;

    /* find all element node, call free_cbk free them */
    struct m_listnode *node = list->head;
    while (node) {
        struct m_listnode *next = node->next;
        node->next = node->prev = NULL;
        if (cbk) cbk(M_LIST_NODE2ELEM(node, list->offset), udt);
        node = next;
    }

    list->offset = 0;
    list->head = list->tail = NULL;
    list->length = 0;

    return 0;
}

int m_list_prepend(struct m_list *list, void *elem)
{
    if (!list || !elem) return M_EINVAL;

    struct m_listnode *node = M_LIST_ELEM2NODE(elem,list->offset);

    node->prev = NULL;
    node->next = list->head;
    if (list->head)
        list->head->prev = node;
    else
        list->tail = node;
    list->head = node;
    list->length++;
    
    return 0;
}

int m_list_append(struct m_list *list, void *elem)
{
    if (!list || !elem) return M_EINVAL;

    struct m_listnode *node = M_LIST_ELEM2NODE(elem,list->offset);

    node->prev = list->tail;
    node->next = NULL;
    if (list->tail)
        list->tail->next = node;
    else
        list->head = node;
    list->tail = node;
    list->length++;

    return 0;
}

size_t m_list_insert(struct m_list *list, void *elem, size_t pos)
{
    size_t real_pos = 0;
    if (!list || !elem) return 0;

    if (pos <= 1) {
        /* if pos<1 insert into start of list */
        m_list_prepend(list, elem);
        real_pos = 1;
    } else if (pos > list->length) {
        /* if pos larger than list length, insert into end of list */
        m_list_append(list, elem);
        real_pos = list->length;
    } else {
        /* find node on the position */
        int i = 1;
        struct m_listnode *node = list->head;
        while (node) {
            if (pos == i) break;
            node = node->next;
            i++;
        }

        if (node) {
            /* insert element into position */
            struct m_listnode *elem_node = M_LIST_ELEM2NODE(elem,list->offset);
            elem_node->prev = node->prev;
            elem_node->next = node;
            node->prev->next = elem_node;
            node->prev = elem_node;
            list->length++;
            real_pos = i;
        } else {
            real_pos = 0;
        }
    }

    return real_pos;
}

int m_list_insert_before(struct m_list *list,void *sibling,void *elem)
{
    if (!list || !sibling || !elem) return M_EINVAL;

    struct m_listnode *sibl_node = M_LIST_ELEM2NODE(sibling,list->offset);
    struct m_listnode *elem_node = M_LIST_ELEM2NODE(elem,list->offset);

    elem_node->prev = sibl_node->prev;
    elem_node->next = sibl_node;
    if (sibl_node->prev)
        sibl_node->prev->next = elem_node;
    else
        list->head = elem_node;
    sibl_node->prev = elem_node;
    list->length++;

    return 0;
}

int m_list_insert_after(struct m_list *list,void *sibling,void *elem)
{
    if (!list || !sibling || !elem) return M_EINVAL;

    struct m_listnode *sibl_node = M_LIST_ELEM2NODE(sibling,list->offset);
    struct m_listnode *elem_node = M_LIST_ELEM2NODE(elem,list->offset);

    elem_node->prev = sibl_node;
    elem_node->next = sibl_node->next;
    if (sibl_node->next)
        sibl_node->next->prev = elem_node;
    else
        list->tail = elem_node;
    sibl_node->next = elem_node;
    list->length++;

    return 0;
}

static int m_list_remove_node(struct m_list *list,
                    struct m_listnode *node)
{
    if (!list || !node) return M_EINVAL;

    if (node->prev)
        node->prev->next = node->next;
    else
        list->head = node->next;
    if (node->next)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;
    list->length--;
    node->prev = node->next = NULL;

    return 0;
}

int m_list_remove(struct m_list *list, void *elem)
{
    if (!list || !elem) return M_EINVAL;        
    return m_list_remove_node(list, M_LIST_ELEM2NODE(elem,list->offset));
}

void *m_list_pop(struct m_list *list, size_t n)
{
    if (!list || !n) return NULL;
    if (n > list->length) return NULL;

    size_t i = 1;
    struct m_listnode *node = list->head;
    for ( ; node && i != n; node = node->next, i++);

    if (node) {
        m_list_remove_node(list, node);
        return M_LIST_NODE2ELEM(node,list->offset);
    }

    return NULL;
}

void *m_list_pop_head(struct m_list *list)
{
    return m_list_pop(list, 1);
}

void *m_list_pop_tail(struct m_list *list)
{
    if (!list) return NULL;

    if (list->tail) {
        struct m_listnode *node = list->tail;
        m_list_remove_node(list, list->tail);
        return M_LIST_NODE2ELEM(node,list->offset);
    }

    return NULL;
}

void *m_list_first(struct m_list *list)
{
    if (!list) return NULL;
    if (!list->head) return NULL;

    return M_LIST_NODE2ELEM(list->head,list->offset);
}

void *m_list_last(struct m_list *list)
{
    if (!list) return NULL;
    if (!list->tail) return NULL;

    return M_LIST_NODE2ELEM(list->tail,list->offset);
}

void *m_list_nth(struct m_list *list, size_t n)
{
    if (!list) return NULL;
    if (n < 1 || n > list->length) return NULL;

    size_t i = 1;
    struct m_listnode *node = list->head;
    
    for ( ; node && i != n; node = node->next, i++);
    if (node)
        return M_LIST_NODE2ELEM(node,list->offset);
    else
        return NULL;
}

void *m_list_prev(struct m_list *list, void *elem)
{
    if (!list || !elem) return NULL;

    struct m_listnode *node = M_LIST_ELEM2NODE(elem,list->offset);
    if (node->prev)
        return M_LIST_NODE2ELEM(node->prev,list->offset);
    else
        return NULL;
}

void *m_list_next(struct m_list *list, void *elem)
{
    if (!list || !elem) return NULL;

    struct m_listnode *node = M_LIST_ELEM2NODE(elem,list->offset);
    if (node->next)
        return M_LIST_NODE2ELEM(node->next,list->offset);
    else
        return NULL;
}

void m_list_travarsal(struct m_list *list, int flag,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!list || !cbk) return;

    struct m_listnode *node = NULL;

    if (flag) {
        node = list->tail;
        while (node) {
            cbk(M_LIST_NODE2ELEM(node,list->offset), udt);
            node = node->prev;
        }
    } else {
        node = list->head;
        while (node) {
            cbk(M_LIST_NODE2ELEM(node,list->offset), udt);
            node = node->next;
        }
    }
}

size_t m_list_index(struct m_list *list, void *elem)
{
    if (!list || !elem) return M_EINVAL;

    size_t i = 1;
    struct m_listnode *node = list->head;
    struct m_listnode *elem_node = M_LIST_ELEM2NODE(elem,list->offset);
    
    for ( ; node && node != elem_node; node = node->next, i++);

    if (node)
        return i;

    return 0;
}

size_t m_list_length(struct m_list *list)
{
     return list ? list->length : 0;
}

void m_list_reverse(struct m_list *list)
{
    if (!list) return;

    struct m_listnode *prev = NULL;
    struct m_listnode *node = list->head;
    while (node) {
        prev = node;
        node = node->next;
        prev->next = prev->prev;
        prev->prev = node;
    }

    list->tail = list->head;
    list->head = prev;
}

int m_list_dup(struct m_list *list, struct m_list *duplist,
                    void *(*cbk)(void *elem, void *udt), void *udt)
{
    if (!list || !duplist || !cbk) return M_EINVAL;

    m_list_init(duplist, list->offset);

    struct m_listnode *node = NULL;
    for (node = list->head; node; node = node->next) {
        void *elem = cbk(M_LIST_NODE2ELEM(node,list->offset), udt);
        if (!elem)
            return M_EUNKNOWN;
        else
            m_list_append(duplist, elem);
    }

    return 0;
}

void *m_list_find(struct m_list *list, void *key,
                int (*cbk)(void *elem, void *key, void *udt), void *udt)
{
    if (!list || !key || !cbk) return NULL;

    struct m_listnode *node = list->head;
    for (node = list->head; node; node = node->next)
        if (cbk(M_LIST_NODE2ELEM(node,list->offset), key, udt) == 0)
            return M_LIST_NODE2ELEM(node,list->offset);

    return NULL;
}

#if 0
static struct sl_listnode *list_sort_merge(struct sl_list *list,
                    struct sl_listnode *n1,
                    struct sl_listnode *n2,
                    int (*compare)(void *a, void *b, void *udt),
                    void *udt)
{
    struct sl_listnode head, *node, *prev;
    int cmp;

    node = &head;
    prev = NULL;
    while (n1 && n2) {
        cmp=compare(SL_LIST_NODE2DATA(n1,list->offset), 
                            SL_LIST_NODE2DATA(n2, list->offset), udt);
        
        if (cmp <= 0) {
            node->next = n1;
            n1 = n1->next;
        } else {
            node->next = n2;
            n2 = n2->next;
        }
        node = node->next;
        node->prev = prev;
        prev = node;
    }
    node->next = n1 ? n1 : n2;
    node->next->prev = node;

    return head.next;
}

static struct sl_listnode *list_sort_split(struct sl_list *list,
                    struct sl_listnode *node,
                    int (*compare)(void *a, void *b, void *udt),
                    void *udt)
{
    struct sl_listnode *n1, *n2;

    if (!node)
        return NULL;
    if (!node->next)
        return node;

    n1 = node;
    n2 = node->next;
    while ((n2 = n2->next) != NULL) {
        if ((n2 = n2->next) == NULL)
            break;
        n1 = n1->next;
    }
    n2 = n1->next;
    n1->next = NULL;

    return list_sort_merge(list, list_sort_split(list, node, compare, udt),
                        list_sort_split(list, n2, compare, udt), compare, udt);
}

int sl_list_sort(struct sl_list *list,
                    int (*compare)(void *a, void *b, void *udt), void *udt)
{
    struct sl_listnode *node = NULL;
    if (!list || !compare) return LIST_EINVAL;

    list->head = list_sort_split(list, list->head, compare, udt);
    if (list->head) {
        for (node = list->head; node->next; node = node->next);
        list->tail = node;
    }

    return 0;
}


int sl_list_insert_sorted(struct sl_list *list, void *data,
                    int (*compare)(void *a, void *b, void *udt), void *udt)
{
    struct sl_listnode *node = NULL;
    if (!list || !compare) return LIST_EINVAL;

    node = list->head;
    while (node) {
        if (compare(SL_LIST_NODE2DATA(node,list->offset), data, udt) < 0) {
            sl_list_insert_before(list,data,SL_LIST_NODE2DATA(node,list->offset));
            break;
        }
        node = node->next;
    }

    if (!node)
        sl_list_append(list, data);

    return 0;
}
#endif
