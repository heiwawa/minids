
#include "circlequeue.h"

int m_cirqueue_init(struct m_cirqueue *que, size_t maxnum)
{
    if (!que || maxnum <= 0) return M_EINVAL;

    que->array = (struct m_quenode *)malloc(sizeof(struct m_quenode)*maxnum);
    if (!que->array)
        return M_EMALLOC;
    que->head = que->tail = 0;
    que->maxnum = maxnum;
    que->num = 0;

    return 0;
}

void m_cirqueue_free(struct m_cirqueue *que,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!que) return;

    /* dequeue all element */
    for ( ; que->num > 0; que->num--) {
        if (cbk) cbk(que->array[que->head].elem, udt);
        que->head = (que->head + 1) % que->maxnum;
    }
}

int m_cirqueue_enque(struct m_cirqueue *que, void *elem,
                    void (*cover)(void *elem, void *udt), void *udt)
{
    if (!que) return M_EINVAL;

    /* if queue is full, dequeue one element */
    if (que->num == que->maxnum) {
        if (cover) cover(que->array[que->head].elem, udt);
        que->head = (que->head + 1) % que->maxnum;
        que->num--;
    }

    /* enqueue */
    que->array[que->tail].elem = elem;
    que->tail = (que->tail + 1) % que->maxnum;
    que->num++;

    return 0;
}

void *m_cirqueue_deque(struct m_cirqueue *que)
{
    void *em = NULL;
    if (!que) return NULL;

    /* queue is empty */
    if (que->num == 0)
        return NULL;

    /* dequeue */
    em = que->array[que->tail].elem;
    que->tail = (que->tail + 1) % que->maxnum;
    que->num--;

    return em;
}
