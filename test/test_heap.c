
#include <stdio.h>
#include <stdlib.h>

#include "heap.h"

struct element {
    int key;
};

int cbk_compare(void *elem1, void *elem2, void *udt)
{
    struct element *em1 = (struct element *)elem1;
    struct element *em2 = (struct element *)elem2;
    if (em1->key > em2->key)
        return 1;
    else if (em1->key < em2->key)
        return -1;
    else 
        return 0;
}

void cbk_free(void *elem, void *udt)
{
    struct element *em = (struct element *)elem;
    printf("free:%d\n", em->key);
    free(elem);
}

int main()
{
    int i = 0;
    int ret = 0;
    struct element *temp = NULL;
    struct m_heap heap = {0};

    ret = m_heap_init(&heap, M_HEAP_MAX, M_HEAP_INC, 100, cbk_compare, NULL);
    if (ret)
        printf("m_heap_init failed:%d\n", ret);

    srand(9);
    for (i = 10; i > 0; i--) {
        int n = rand() % 100;
        struct element *elm = (struct element *)malloc(sizeof(struct element));
        elm->key = n;
        if (n == 15) temp = elm;
        /* printf("m_avltree_insert %d\n", n); */
        ret = m_heap_insert(&heap, elm);
        if (ret)
            printf("m_rbtree_insert failed:%d\n", ret);
    }

    if (m_heap_judge(&heap))
        printf("is not a heap\n");
    else
        printf("is a heap\n");

    /* remove test */
    ret = m_heap_remove(&heap, temp);
    if (ret)
        printf("m_heap_remove failed:%d\n", ret);
    else
        free(temp);
#if 0
    printf("m_heap_peek test:\n");
    do {
        temp = m_heap_peek(&heap);
        if (temp)
            printf("%d ", temp->key);
        temp = m_heap_pop(&heap);
        if (temp)
            free(temp);
    } while (temp);
    printf("\n");
#endif
#if 1
    printf("m_heap_pop test:\n");
    do {
        temp = m_heap_pop(&heap);
        if (temp) {
            printf("%d ", temp->key);
            free(temp);
        }
    } while (temp);
    printf("\n");
#endif

    if (m_heap_judge(&heap))
            printf("is not a heap\n");
        else
            printf("is a heap\n");

    m_heap_free(&heap, cbk_free, NULL);

    return 0;
}
