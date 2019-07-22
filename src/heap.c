
#include "heap.h"

#define PARENT(i) ((i - 1) / 2)
#define LCHILD(i) ((2 * i) + 1)
#define RCHILD(i) ((2 * i) + 2)

#define SHIFT(a,b) \
    do { \
        void *tmp = NULL; \
        tmp = heap->array[a].elem; \
        heap->array[a].elem = heap->array[b].elem; \
        heap->array[b].elem = tmp; \
    } while (0)

static void shift_up(struct m_heap *heap, size_t i)
{
    if (i <= 0) return;
    while (PARENT(i) >= 0) {
        int ret = heap->compare(heap->array[PARENT(i)].elem,heap->array[i].elem,
                            heap->udt);
        if ((ret > 0 && heap->type == M_HEAP_MIN) ||
            (ret < 0 && heap->type == M_HEAP_MAX)) {
            SHIFT(PARENT(i),i);
            i = PARENT(i);
            if (i <= 0)
                break;
        } else {
            break;
        }
    }
}

static void shift_down(struct m_heap *heap, size_t i)
{
    int ret = 0;
    size_t shift = 0;
    while (LCHILD(i) < heap->num) {
        shift = LCHILD(i);
        /* compare left right child first */
        if (RCHILD(i) < heap->num) {
            ret = heap->compare(heap->array[LCHILD(i)].elem,
                                heap->array[RCHILD(i)].elem, heap->udt);
            if ((ret > 0 && heap->type == M_HEAP_MIN) ||
                (ret < 0 && heap->type == M_HEAP_MAX))
                shift = RCHILD(i);
        }
        /* comprare parent child */
        ret = heap->compare(heap->array[i].elem,
                            heap->array[shift].elem, heap->udt);
        if ((ret > 0 && heap->type == M_HEAP_MIN) ||
            (ret < 0 && heap->type == M_HEAP_MAX)) {
            SHIFT(i,shift);
            i = shift;
        } else {
            break;
        }
    }
}

int m_heap_init(struct m_heap *heap, int type, int flag, size_t maxnum,
                int (*compare)(void *elem1, void *elem2, void *udt), void *udt)
{
    if (!heap || (type != M_HEAP_MIN && type != M_HEAP_MAX) ||
        (flag !=M_HEAP_NOINC && flag !=M_HEAP_INC) || maxnum <= 0 || !compare)
        return M_EINVAL;

    heap->array = (struct m_heapnode *)malloc(sizeof(struct m_heapnode)*maxnum);
    if (heap->array == NULL)
        return M_EMALLOC;
    
    heap->type = type;
    heap->flag = flag;
    heap->maxnum = maxnum;
    heap->num = 0;
    heap->compare = compare;
    heap->udt = udt;

    return 0;
}

void m_heap_free(struct m_heap *heap,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    size_t i = 0;
    if (!heap) return;

    if (cbk)
        for (i = 0; i < heap->num; i++)
            cbk(heap->array[i].elem, udt);
    free(heap->array);
    heap->array = NULL;
    heap->type = 0;
    heap->flag = 0;
    heap->maxnum = 0;
    heap->num = 0;
    heap->compare = NULL;
    heap->udt = NULL;
}

int m_heap_insert(struct m_heap *heap, void *elem)
{
    if (!heap) return M_EINVAL;

    if (heap->num >= heap->maxnum) {
        if (heap->flag == M_HEAP_NOINC) {
            return M_ETOOMANY;
        } else {
            /* reallocate memory */
            size_t newsize = heap->maxnum * 2;
            void *p = realloc(heap->array, sizeof(struct m_heapnode)*newsize);
            if (!p)
                return M_EMALLOC;
            
            heap->maxnum = newsize;
            heap->array = (struct m_heapnode *)p;
        }
    }

    heap->array[heap->num].elem = elem;
    shift_up(heap, heap->num);
    heap->num++;

    return 0;
}

void *m_heap_peek(struct m_heap *heap)
{
    if (!heap) return NULL;
    if (heap->num <= 0) return NULL;

    return heap->array[0].elem;
}

void *m_heap_pop(struct m_heap *heap)
{
    void *elem = NULL;
    if (!heap) return NULL;
    if (heap->num <= 0) return NULL;

    elem = heap->array[0].elem;
    heap->array[0].elem = heap->array[heap->num - 1].elem;
    heap->num--;
    shift_down(heap, 0);

    return elem;
}

int m_heap_remove(struct m_heap *heap, void *elem)
{
    size_t i = 0;
    int ret = 0;
    if (!heap) return M_EINVAL;

    /* find elem position */
    for (i = 0; i < heap->num; i++) {
        ret = heap->compare(heap->array[i].elem, elem, heap->udt);
        if (ret == 0)
            break;
    }
    if (i == heap->num)
        return M_ENOTFOUND;
    
    heap->array[i].elem = heap->array[heap->num - 1].elem;
    heap->num--;
    if (i > 0 && PARENT(i) <= heap->num) {
        ret = heap->compare(heap->array[PARENT(i)].elem,
                            heap->array[i].elem, heap->udt);
        if ((ret > 0 && heap->type == M_HEAP_MIN) ||
            (ret < 0 && heap->type == M_HEAP_MAX)) {
            shift_up(heap, i);
            return 0;
        }
    }
    shift_down(heap, i);

    return 0;
}

int m_heap_judge(struct m_heap *heap) {
    size_t i = 0;
    int ret = 0;
    if (!heap) return -1;
    
    for (i = 0; i < heap->num; i++) {
        if (LCHILD(i) < heap->num) {
            ret = heap->compare(heap->array[i].elem,
                                heap->array[LCHILD(i)].elem, heap->udt);
            if ((ret > 0 && heap->type == M_HEAP_MIN) ||
                (ret < 0 && heap->type == M_HEAP_MAX))
                return -1;
        }
        if (RCHILD(i) < heap->num) {
            ret = heap->compare(heap->array[i].elem,
                                heap->array[RCHILD(i)].elem, heap->udt);
            if ((ret > 0 && heap->type == M_HEAP_MIN) ||
                (ret < 0 && heap->type == M_HEAP_MAX))
                return -1;
        }
    }

    return 0;
}
