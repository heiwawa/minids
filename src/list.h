/****************************************************
* Copyright (c) 2019 dangqian All rights reserved.
* @brief    duble linked list
*****************************************************/

#ifndef _MINIDS_LIST_H_
#define _MINIDS_LIST_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MINIDS_ERROR
#define MINIDS_ERROR
#define M_EINVAL    (-1) /* invalid arguments */
#define M_EUNKNOWN  (-2) /* unknown error */
#define M_ENOTFOUND (-3) /* not found */
#endif

/*******************************************************
 * @brief   calculate listnode offset in element, just use for m_list_init()
 * @TYPE    element type
 * @MEMBER  listnode
 * @sample  struct element {
 *              int key;
 *              struct m_listnode listnode;
 *          }
 *          M_LIST_OFFSET(struct element, listnode)
********************************************************/
#define M_LIST_OFFSET(TYPE, MEMBER) ((size_t)&((TYPE *)0)->MEMBER)

/********************************************************
 * @brief   get listnode from element
 * @DATA    element instance addr
 * @OFFSET  list.offset
 * @sample  struct m_listnode *node = M_LIST_ELEM2NODE(elem, list.offset);
*********************************************************/
#define M_LIST_ELEM2NODE(ELEM,OFFSET) \
                    ((struct m_listnode *)((size_t)(ELEM) + (OFFSET)))

/********************************************************
 * @brief   get element instance addr from listnode
 * @NODE    listnode
 * @OFFSET  list.offset
 * @sample  struct element *elem = M_LIST_NODE2ELEM(listnode, list.offset);
*********************************************************/
#define M_LIST_NODE2ELEM(NODE,OFFSET) ((void *)((size_t)(NODE) - (OFFSET)))

struct m_listnode {
    struct m_listnode *prev;
    struct m_listnode *next;
};

struct m_list {
    struct m_listnode *head;
    struct m_listnode *tail;
    size_t offset; /* listnode offset in element */
    size_t length; /* list length, number of elements */
};

/********************************************************
 * @brief   initialize list instance
 * @list    list instance addr
 * @offset  listnode offset in element
 * @return  0 success, M_EXXX otherwise
 * @sample  struct element {
 *              int key;
 *              struct m_listnode listnode;
 *          }
 *          struct m_list *list = malloc(1, sizeof(struct m_list));
 *          m_list_init(list, M_LIST_OFFSET(struct element, listnode));
*********************************************************/
int m_list_init(struct m_list *list, size_t offset);

/*******************************************************
 * @brief   reset list instance, free memory of element in list 
 *          by 'free' callback
 * @list    list instance addr
 * @cbk     callback function use for free element memory
 *          NOTE! if NULL may cause memory leak
 * @udt     opaque pram to callback
 * @return  0 success, M_EXXX otherwise
 * @sample  void cbk_free(void *elem, void *udt)
 *          {
 *              struct element *em = (struct element *)elem;
 *              void *userdata = udt;
 *              free(em);
 *          }
 *          m_list_free(list, cbk_free, userdata);
********************************************************/
int m_list_free(struct m_list *list,
                    void (*cbk)(void *elem, void *udt), void *udt);

/********************************************************
 * @brief   prepends a new element to the start of list
 * @list    list instance
 * @elem    the new element
 * @return  0 success, M_EXXX otherwise
********************************************************/
int m_list_prepend(struct m_list *list, void *elem);

/*******************************************************
 * @brief   append a new element to the end of list
 * @list    list instance
 * @elem    the new element
 * @return  0 success, M_EXXX otherwise
********************************************************/
int m_list_append(struct m_list *list, void *elem);

/*******************************************************
 * @brief   insert a new element into the list at the give position
 * @list    list instance
 * @elem    the new element
 * @pos     position, head pos is 1, if pos<1 will insert into start of list,
 *          if pos larger than list length will insert into end of list
 * @return  if success return real inserted position, 0 otherwise
********************************************************/
size_t m_list_insert(struct m_list *list, void *elem, size_t pos);

/*******************************************************
 * @brief   insert a new element before the sibling element
 * @list    list instance
 * @sibling sibling element must in the list
 * @elem    the new element
 * @return  0 success, M_EXXX otherwise
********************************************************/
int m_list_insert_before(struct m_list *list, void *sibling,
                    void *elem);

/*******************************************************
 * @brief   insert a new element after the sibling element
 * @list    list instance
 * @sibling sibling element must in the list
 * @elem    the new element
 * @return  0 success, M_EXXX otherwise
********************************************************/
int m_list_insert_after(struct m_list *list,void *sibling,void *elem);

/*******************************************************
 * @brief   remove an element from list, it's just remove not free any memory
 * @list    list instance
 * @elem    the element will remove, element must in list
 * @return  0 success, M_EXXX otherwise
********************************************************/
int m_list_remove(struct m_list *list, void *elem);

/*******************************************************
 * @brief   popup n-th element
 * @list    list instance
 * @n       n-th, first element is 1, if out of range will return NULL
 * @return  element addr, NULL otherwise
********************************************************/
void *m_list_pop(struct m_list *list, size_t n);

/*******************************************************
 * @brief   popup first element
 * @list    list instance
 * @return  element addr, NULL otherwise
********************************************************/
void *m_list_pop_head(struct m_list *list);

/*******************************************************
 * @brief   popup last element
 * @list    list instance
 * @return  element addr, NULL otherwise
********************************************************/
void *m_list_pop_tail(struct m_list *list);

/*******************************************************
 * @brief   get first element of list
 * @list    list instance
 * @return  element addr, NULL otherwise
********************************************************/
void *m_list_first(struct m_list *list);

/*******************************************************
 * @brief   get last element of list
 * @list    list instance
 * @return  element addr, NULL otherwise
********************************************************/
void *m_list_last(struct m_list *list);

/*******************************************************
 * @brief   get n-th element of list
 * @list    list instance
 * @n       n-th, first element is 1, if out of range will return NULL
 * @return  element addr, NULL otherwise
********************************************************/
void *m_list_nth(struct m_list *list, size_t n);

/*******************************************************
 * @brief   get prev element of the give element
 * @list    list instance
 * @elem    must in list, if elem is first return NULL
 * @return  element addr, NULL otherwise
********************************************************/
void *m_list_prev(struct m_list *list, void *elem);

/*******************************************************
 * @brief   get next element of the give element
 * @list    list instance
 * @elem    must in list, if elem is last return NULL
 * @return  element addr, NULL otherwise
********************************************************/
void *m_list_next(struct m_list *list, void *elem);

/*******************************************************
 * @brief   traversal list forward or backward
 * @list    list instance
 * @falg    0 forward(traversal from head), 1 backward(traversal from tail)
 * @cbk     callback function, callback each element
 * @udt     opaque pram to callback
********************************************************/
void m_list_travarsal(struct m_list *list, int flag,
                    void (*cbk)(void *elem, void *udt), void *udt);

/*******************************************************
 * @brief   get index of give element in the list
 * @list    list instance
 * @elem    if element is not in list return 0
 * @return  index, 0 not found
********************************************************/
size_t m_list_index(struct m_list *list, void *elem);

/*******************************************************
 * @brief   get list length
 * @list    list instance
 * @return  list length
********************************************************/
size_t m_list_length(struct m_list *list);

/*******************************************************
 * @brief   reverse whole list
 * @list    list instance
********************************************************/
void m_list_reverse(struct m_list *list);

/*******************************************************
 * @brief   duplicate whole list
 * @list    list instance addr
 * @duplist new list addr, memory must allocated
 * @cbk     callback function use for duplicate element in list
 *          return the duplicated element
 * @udt     opaque pram to callback
 * @sample  void *cbk_dup(void *elem, void *udt)
 *          {
 *              struct element *em = malloc(sizeof(struct element));
 *              void *userdata = udt;
 *              memcpy(em, elem);
 *              return em;
 *          }
 *          m_list_dup(list, duplist, cbk_dup, userdata);
 * @return  0 success, M_EXXX otherwise
********************************************************/
int m_list_dup(struct m_list *list, struct m_list *duplist,
                    void *(*cbk)(void *elem, void *udt), void *udt);

/*******************************************************
 * @brief   find an element by given key
 * @list    list instance addr
 * @key     the key use for confirm an specific element, 
 *          will pass to callback funtion
 * @cbk     callback function use for confirm an specific element
 *          if it is element to look for then return 0, otherwise return -1
 * @udt     opaque pram pass to callback
 * @sample  int cbk_find(void *elem, void *key, void *udt)
 *          {
 *              if elem->key == key return 0;
 *              else return -1;
 *          }
 *          m_list_dup(list, duplist, cbk_dup, userdata);
 * @return  element addr, NULL otherwise
********************************************************/
void *m_list_find(struct m_list *list, void *key,
                int (*cbk)(void *elem, void *key, void *udt), void *udt);

/*
int sl_list_sort(struct sl_list *list,
                    int (*compare)(void *a, void *b, void *udt), void *udt);
                    
int sl_list_insert_sorted(struct sl_list *list, void *data,
                    int (*compare)(void *a, void *b, void *udt), void *udt);
*/
#ifdef __cplusplus
}
#endif

#endif

