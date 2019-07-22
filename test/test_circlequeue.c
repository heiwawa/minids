
#include <stdio.h>

#include "circlequeue.h"

struct element {
    int key;
};

void cbk_free(void * elem,void * udt)
{
    struct element *em = (struct element *)elem;
    printf("free:%d\n", em->key);
    free(em);
}

void cbk_cover(void * elem,void * udt)
{
    struct element *em = (struct element *)elem;
    printf("cover:%d\n", em->key);
    free(em);
}

int main()
{
    int i = 0;
    int ret = 0;
    struct m_cirqueue que = {0};

    ret = m_cirqueue_init(&que, 10);
    if (ret)
        printf("m_cirqueue_init failed:%d\n", ret);
    else
        printf("m_cirqueue_init success\n");

    srand(5);
    for (i = 21; i > 0; i--) {
        int n = rand() % 100;
        struct element *elm = (struct element *)malloc(sizeof(struct element));
        elm->key = n;
        /* printf("m_avltree_insert %d\n", n); */
        ret = m_cirqueue_enque(&que, elm, cbk_cover, NULL);
        if (ret)
            printf("m_cirqueue_enque failed:%d\n", ret);
        else
            printf("m_cirqueue_enque:%d\n", n);
    }
    
    struct element *temp = NULL;
    do {
        temp = m_cirqueue_deque(&que);
        if (temp) {
            printf("m_cirqueue_deque:%d\n", temp->key);
            free(temp);
        }
    } while (temp);

    m_cirqueue_free(&que, cbk_free, NULL);

    return 0;
}
