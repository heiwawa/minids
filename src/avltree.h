/****************************************************
* Copyright (c) 2019 dangqian All rights reserved.
* @brief    AVL binary search tree
*****************************************************/

#ifndef __MINIDS_AVLTREE_H__
#define __MINIDS_AVLTREE_H__

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
 * @brief   calculate avlnode offset in element, just use for m_avltree_init()
 * @TYPE    element type
 * @MEMBER  avlnode
 * @sample  struct element {
 *              int key;
 *              struct m_avlnode avlnode;
 *          }
 *          M_AVLTREE_OFFSET(struct element, avlnode)
********************************************************/
#define M_AVLTREE_OFFSET(TYPE,MEMBER) ((size_t)&((TYPE *)0)->MEMBER)

struct m_avlnode {
    struct m_avlnode *left;
    struct m_avlnode *right;
    struct m_avlnode *parent;
    unsigned int height;
};

struct m_avltree {
    struct m_avlnode *root;
    size_t offset;
    size_t count; /* node count of tree */
};

/********************************************************
 * @brief   initialize avltree
 * @tree    avltree instance addr
 * @offset  avlnode offset in element
 * @return  0 success, M_EXXX otherwise
 * @sample  struct element {
 *              int key;
 *              struct m_avlnode avlnode;
 *          }
 *          struct m_avltree *avltree = malloc(1, sizeof(struct m_avltree));
 *          m_avltree_init(avltree, M_AVLTREE_OFFSET(struct element, avlnode));
*********************************************************/
int m_avltree_init(struct m_avltree *tree, size_t offset);

/*******************************************************
 * @brief   reset avltree, free memory of element in avltree 
 *          by 'free' callback
 * @tree    avltree instance addr
 * @cbk     callback function use for free element memory, can't NULL
 * @udt     opaque pram to callback
 * @return  0 sucess, M_Exxx otherwise
 * @sample  void cbk_free(void *elem, void *udt)
 *          {
 *              struct element *em = (struct element *)elem;
 *              void *userdata = udt;
 *              free(em);
 *          }
 *          m_avltree_free(tree, cbk_free, userdata);
********************************************************/
int m_avltree_free(struct m_avltree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt);

#if 0
typedef struct _sl_avltree *sl_avltree;

int sl_avltree_insert(sl_avltree *pavl, void *elem,
                int (*compare)(void *ielem, void *elem, void *udt), void *udt);

int sl_avltree_remove(sl_avltree *pavl, void *key,
                int (*compare)(void *ielem, void *key, void *udt), void *udt);

void *sl_avltree_find(sl_avltree avl, void *key,
                int (*compare)(void *ielem, void *key, void *udt), void *udt);

void *sl_avltree_prev(sl_avltree avl, void *key,
                int (*compare)(void *ielem, void *key, void *udt), void *udt);

void *sl_avltree_next(sl_avltree avl, void *key,
                int (*compare)(void *ielem, void *key, void *udt), void *udt);

void *sl_avltree_first(sl_avltree avl);

void *sl_avltree_last(sl_avltree avl);

void sl_avltree_free(sl_avltree *pavl,
                    void (*free)(void *elem, void *udt), void *udt);

int sl_avltree_judge(sl_avltree avl, int *height);

#ifdef __SLLIB_H__
void sl_avltree_print(sl_avltree avl,
                    void (*func)(int newline, void *data));
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif

