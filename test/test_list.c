

#include <stdio.h>
#include <string.h>

#include "list.h"

struct element {
    int key;
    struct m_listnode listnode;
};


static void free_cbk(void *element, void *udt)
{
    struct element *elem = (struct element *)element;
    /* printf("free_cbk %d\n", elem->key); */
    free(elem);
}

static void trav_cbk(void *element, void *udt)
{
    struct element *elem = (struct element *)element;
    printf("trav_cbk %d\n", elem->key);
}

void *dup_cbk(void *elem, void *udt)
{
    struct element *em = malloc(sizeof(struct element));
    memcpy(em, elem, sizeof(struct element));

    return em;
}

static int find_cbk(void *elem, void *key, void *udt)
{
    struct element *em = (struct element *)elem;
    int k = (int)(long)key;

    if (em->key == k) return 0;
    else return -1;
}

int main()
{
    int i = 0;
    int ret = 0;
    struct m_list list = {0};
    struct element *elem = NULL;

    /* initialize list */
    ret = m_list_init(&list, M_LIST_OFFSET(struct element, listnode));
    if (ret) {
        printf("m_list_init failed: %d\n", ret);
        return -1;
    }
    
#if 0
    /* prepend element */
    for (i = 0; i < 3; i++) {
        elem = malloc(sizeof(struct element));
        elem->key = i;
        ret = m_list_prepend(&list, elem);
        if (ret) {
            printf("m_list_prepend failed: %d\n", ret);
            break;
        }
    }
#endif

#if 1
    /* append element */
    for (i = 0; i < 3; i++) {
        elem = malloc(sizeof(struct element));
        elem->key = i;
        ret = m_list_append(&list, elem);
        if (ret) {
            printf("m_list_append failed: %d\n", ret);
            break;
        }
    }
#endif

#if 1
    /* insert element */
    elem = malloc(sizeof(struct element));
    elem->key = 999;
    ret = m_list_insert(&list, elem, 2);
    if (ret < 0)
        printf("m_list_insert failed: %d\n", ret);
    else
        printf("m_list_insert return: %d\n", ret);
#endif

#if 0
    struct element *elem1 = NULL;
    elem1 = malloc(sizeof(struct element));
    elem1->key = 888;
    ret = m_list_insert_before(&list, elem, elem1);
    if (ret < 0)
        printf("m_list_insert_before failed: %d\n", ret);
#endif

#if 0
    struct element *elem1 = NULL;
    elem1 = malloc(sizeof(struct element));
    elem1->key = 888;
    ret = m_list_insert_after(&list, elem, elem1);
    if (ret < 0)
        printf("m_list_insert_after failed: %d\n", ret);
    
#endif
    m_list_travarsal(&list, 0, trav_cbk, NULL);
#if 0
    ret = m_list_remove(&list, elem);
    if (ret < 0)
        printf("m_list_remove failed: %d\n", ret);
    printf("m_list_remove %d\n", elem->key);
    free(elem);

    m_list_travarsal(&list, 0, trav_cbk, NULL);
#endif

#if 0
    elem = m_list_first(&list);
    printf("m_list_first %d\n", elem->key);
    elem = m_list_last(&list);
    printf("m_list_last %d\n", elem->key);
    elem = m_list_nth(&list, 2);
    printf("m_list_nth %d\n", elem->key);
    struct element *tmp = m_list_prev(&list, elem);
    printf("m_list_prev %d\n", tmp->key);
    tmp = m_list_next(&list, elem);
    printf("m_list_next %d\n", tmp->key);
#endif

#if 0
    printf("element index:%ld\n", m_list_index(&list, elem));

    elem = m_list_pop(&list, 4);
    printf("m_list_pop:%d\n", elem->key);
    elem = m_list_pop_head(&list);
    printf("m_list_pop_head:%d\n", elem->key);
    elem = m_list_pop_tail(&list);
    printf("m_list_pop_tail:%d\n", elem->key);

    printf("list length:%d\n", (int)list.length);
#endif
#if 0
    m_list_reverse(&list);
    printf("reverse\n");
    m_list_travarsal(&list, 0, trav_cbk, NULL);
#endif
#if 0
    struct m_list duplist;
    ret = m_list_dup(&list, &duplist, dup_cbk, NULL);
    if (ret) printf("m_list_dup failed %d\n", ret);
    printf("m_list_dup duplist\n");
    m_list_travarsal(&duplist, 0, trav_cbk, NULL);
#endif

    elem = m_list_find(&list, (void *)(long)2, find_cbk, NULL);
    if (!elem) printf("m_list_find failed\n");
    else printf("m_list_find:%d\n", elem->key);

    m_list_free(&list, free_cbk, NULL);

/*
    int i = 0;
    struct sl_list list;
    struct object *tail = NULL;
    struct object *sib = malloc(sizeof(*sib));
    sib->key = 1;
    struct object *obj = malloc(sizeof(*obj));
    obj->key = 111;

    sl_list_init(&list, SL_LIST_OFFSET(struct object,listnode));

    srand(5);
    for (i = 0; i < 10; i++) {
        int n = rand() % 100;
        printf("n:%d\n", n);
        struct object *obj = malloc(sizeof(*obj));
        obj->key = n;
        sl_list_prepend(&list, obj);
    }

    sl_list_foreach(&list, list_foreach, NULL);
    
    printf("insert to %d\n", sl_list_insert(&list, sib, 2));
    sl_list_foreach(&list, list_foreach, NULL);
    tail = sl_list_last(&list);
    printf("list tail: %d\n", tail->key);
    
    printf("insert after 1\n");
    sl_list_insert_after(&list, obj, sib);
    sl_list_foreach(&list, list_foreach, NULL);
    tail = sl_list_last(&list);
    printf("list tail: %d\n", tail->key);
    
    printf("remove 111\n");
    sl_list_remove(&list, obj);
    sl_list_foreach(&list, list_foreach, NULL);
    tail = sl_list_last(&list);
    printf("list tail: %d\n", tail->key);
    
    printf("reverse\n");
    sl_list_reverse(&list);
    sl_list_foreach(&list, list_foreach, NULL);

    tail = sl_list_last(&list);
    printf("list tail: %d\n", tail->key);

    i = 11;
    obj = sl_list_nth(&list, i);
    if (obj)
        printf("list nth %d:%d\n", i, obj->key);
    else
        printf("can not find nth %d\n", i);

    printf("list obj index:%d\n", sl_list_index(&list, obj));

    printf("list lenght:%ld\n", sl_list_length(&list));

    i = 12;
    obj = sl_list_pop(&list, i);
    if (obj)
        printf("list pop %d:%d\n", i, obj->key);
    else
        printf("can not find nth %d\n", i);
    sl_list_foreach(&list, list_foreach, NULL);
    tail = sl_list_last(&list);
    printf("list tail: %d\n", tail->key);

    printf("after sort\n");
    sl_list_sort(&list, list_compare, NULL);
    sl_list_foreach(&list, list_foreach, NULL);
    tail = sl_list_last(&list);
    printf("list tail: %d\n", tail->key);

    obj = sl_list_pop_head(&list);
    if (obj)
        printf("list pop head %d\n", obj->key);
    else
        printf("head is NULL\n");
    sl_list_foreach(&list, list_foreach, NULL);
    tail = sl_list_last(&list);
    printf("list tail: %d\n", tail->key);

    obj = sl_list_pop_tail(&list);
    if (obj)
        printf("list pop tail %d\n", obj->key);
    else
        printf("tail is NULL\n");
    sl_list_foreach(&list, list_foreach, NULL);
    tail = sl_list_last(&list);
    printf("list tail: %d\n", tail->key);

    struct sl_list duplist;
    sl_list_dup(&list, &duplist, list_dup, NULL);
    printf("duplist:\n");
    sl_list_foreach(&duplist, list_foreach, NULL);
    tail = sl_list_last(&duplist);
    printf("list tail: %d\n", tail->key);

    sl_list_free(&list, list_free, NULL);
    sl_list_free(&duplist, list_free, NULL);
*/
    return 0;
}
