/****************************************************
* Copyright (c) 2019 dangqian All rights reserved.
* @brief    circle queue
*****************************************************/

#ifndef __MINIDS_CIRCLEQUEUE_H__
#define __MINIDS_CIRCLEQUEUE_H__

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

struct m_quenode {
    void *elem;
};

struct m_cirqueue {
    struct m_quenode *array;
    size_t head;
    size_t tail;
    size_t maxnum;
    size_t num;
};

/********************************************************
 * @brief   initialize queue
 * @que     queue instance addr
 * @maxnum  max numbers of element in queue
 * @return  0 success, M_EXXX otherwise
*********************************************************/
int m_cirqueue_init(struct m_cirqueue *que, size_t maxnum);

/*******************************************************
 * @brief   reset queue, free memory by callback 'free'
 * @que     queue instance addr
 * @free    memory free callback, iterate all element
 *          NOTE! if NULL may cause memory leak
 *          @elem   element in heap
 *          @udt    opaque userdata
 *          @sample
 *          void cbk_free(void *elem, void *udt)
 *          {
 *              struct element *em = (struct element *)elem;
 *              if (em) {
 *                  TODO;
 *                  free(em);
 *              }
 *          }
 * @udt     opaque param pass to callback
********************************************************/
void m_cirqueue_free(struct m_cirqueue *que,
                    void (*free)(void *elem, void *udt), void *udt);

/*******************************************************
 * @brief   enqueue an new element
 * @que     queue instance addr
 * @elem    the new element
 * @cover   element cover callback, if queue is full, then enqueue will
 *          cause new elem cover older one, 'cover' will callback older
 *          element to you for do someting, such as free oder elem memory
 *          NOTE! if NULL may cause memory leak
 *          @elem   element in queue
 *          @udt    opaque userdata
 *          @sample
 *          void cbk_cover(void *elem, void *udt)
 *          {
 *              struct element *em = (struct element *)elem;
 *              if (em) {
 *                  TODO;
 *                  free(em);
 *              }
 *          }
 * @return  0 sucess, M_Exxx otherwise
********************************************************/
int m_cirqueue_enque(struct m_cirqueue *que, void *elem,
                    void (*cover)(void *elem, void *udt), void *udt);

/********************************************************
 * @brief   dequeue
 * @que     queue instance addr
 * @return  element addr, NULL is error or empty queue
*********************************************************/
void *m_cirqueue_deque(struct m_cirqueue *que);

#ifdef __cplusplus
}
#endif

#endif
