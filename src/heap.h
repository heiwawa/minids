/****************************************************
* Copyright (c) 2019 dangqian All rights reserved.
* @brief    min-heap / max-heap
*****************************************************/

#ifndef __MINIDS_HEAP_H__
#define __MINIDS_HEAP_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MINIDS_ERROR
#define MINIDS_ERROR
#define M_EINVAL    (-1) /* invalid arguments */
#define M_EUNKNOWN  (-2) /* unknown error */
#define M_ENOTFOUND (-3) /* not found */
#define M_EEXISTS   (-4) /* equal key of element already exist */
#define M_ECALLBACK (-5) /* cbk function return error result */
#define M_ETOOMANY  (-6) /* too many element */
#define M_EMALLOC   (-7) /* memory allocate failed */
#endif

#define M_HEAP_MIN 0 /* min-heap */
#define M_HEAP_MAX 1 /* max-heap */
#define M_HEAP_INC      1 /* auto incrementally */
#define M_HEAP_NOINC    0 /* no increment */

struct m_heapnode {
    void *elem;
};

/********************************************************
 * @brief   heap struct define
 * @type    M_HEAP_MIN or M_HEAP_MAX
 * @flag    M_HEAP_NOINC or M_HEAP_INC
 *          NOINC: when reach maxnum, insert will failed and return M_ETOOMANY
 *          INC: when reach maxnum, insert will free old allocated memory
 *          reallocate double maxnum memory and insert new element
 * @maxnum  max numbers of element in heap, if M_HEAP_INC is set, will auto
 *          double increment
 * @num     counts of element in heap
 * @array   memory to store element
 * @compare callback function compare two element sequence
 *          @sample
 *          void compare(void *elem1, void *elem2, void *udt)
 *          {
 *              struct element *em1 = (struct element *)elem1;
 *              struct element *em2 = (struct element *)elem2;
 *              if (em1->key > em2->key)
 *                  return 1;
 *              else if (em1->key < em2->key)
 *                  return -1;
 *              else
 *                  return 0;
 *          }
 * @udt     opaque param pass to callback
*********************************************************/
struct m_heap {
    int type;
    int flag;
    size_t maxnum;
    size_t num;
    struct m_heapnode *array;
    int (*compare)(void *elem1, void *elem2, void *udt);
    void *udt;
};

/********************************************************
 * @brief   initialize heap
 * @heap    heap instance addr
 * @type    M_HEAP_MIN or M_HEAP_MAX
 * @flag    M_HEAP_NOINC or M_HEAP_INC
 * @maxnum  max numbers of element in heap
 * @compare callback function compare two element sequence
 * @udt     opaque param pass to callback
 * @return  0 success, M_EXXX otherwise
*********************************************************/
int m_heap_init(struct m_heap *heap, int type, int flag, size_t maxnum,
                int (*compare)(void *elem1, void *elem2, void *udt), void *udt);

/*******************************************************
 * @brief   reset heap, free memory by 'cbk_free'
 * @heap    heap instance addr
 * @cbk     memory free callback, iterate all element
 *          NOTE! if NULL may cause memory leak
 *          @elem   element in heap
 *          @udt    opaque userdata
 *          @sample
 *          void cbk_free(void *elem, void *udt)
 *          {
 *              struct element *em = (struct element *)elem;
 *              if (em) {
 *                  TODO;free(em);
 *              }
 *          }
 * @udt     opaque param pass to callback
********************************************************/
void m_heap_free(struct m_heap *heap,
                    void (*cbk)(void *elem, void *udt), void *udt);

/*******************************************************
 * @brief   insert an new element into heap
 * @heap    heap instance addr
 * @elem    the new element
 * @return  0 sucess, M_Exxx otherwise
********************************************************/
int m_heap_insert(struct m_heap *heap, void *elem);

/********************************************************
 * @brief   peek heap head
 * @heap    heap instance addr
 * @return  heap head element, NULL otherwise
*********************************************************/
void *m_heap_peek(struct m_heap *heap);

/********************************************************
 * @brief   pop heap head
 * @heap    heap instance addr
 * @return  heap head element, NULL otherwise
*********************************************************/
void *m_heap_pop(struct m_heap *heap);

/*******************************************************
 * @brief   remove an new element from heap
 * @heap    heap instance addr
 * @elem    the element
 * @return  0 sucess, M_Exxx otherwise
********************************************************/
int m_heap_remove(struct m_heap *heap, void *elem);

/********************************************************
 * @brief   judge a m_heap is min-heap/max-heap
 * @heap    heap instance addr
 * @return  0 is min-heap/max-heap,
 *          otherwise is not a min-heap neither max-heap
*********************************************************/
int m_heap_judge(struct m_heap *heap);

#ifdef __cplusplus
}
#endif

#endif
