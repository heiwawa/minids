/****************************************************
* Copyright (c) 2019 dangqian All rights reserved.
* @brief    Red Black binary tree
*****************************************************/

#ifndef __MINIDS_RBTREE_H__
#define __MINIDS_RBTREE_H__

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
#endif

/*******************************************************
 * @brief   calculate rbnode offset in element, just use for m_rbtree_init()
 * @TYPE    element type
 * @MEMBER  rbnode
 * @sample  struct element {
 *              int key;
 *              struct m_rbnode rbnode;
 *          }
 *          M_RBTREE_OFFSET(struct element, rbnode)
********************************************************/
#define M_RBTREE_OFFSET(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

struct m_rbnode {
    struct m_rbnode *left;
    struct m_rbnode *right;
    struct m_rbnode *parent;
    unsigned int color;
};

struct m_rbtree {
    struct m_rbnode *root;
    size_t offset;
    size_t count;
};

/********************************************************
 * @brief   initialize rbtree
 * @tree    rbtree instance addr
 * @offset  rbnode offset in element
 * @return  0 success, M_EXXX otherwise
 * @sample  struct element {
 *              int key;
 *              struct m_rbnode rbnode;
 *          }
 *          struct m_rbtree *rbtree = malloc(1, sizeof(struct m_rbtree));
 *          m_rbtree_init(rbtree, M_RBTREE_OFFSET(struct element, rbnode));
*********************************************************/
int m_rbtree_init(struct m_rbtree *tree, size_t offset);

/*******************************************************
 * @brief   reset rbtree, free memory of element in rbtree 
 *          by 'free' callback
 * @tree    rbtree instance addr
 * @cbk     callback function use for free element memory
 *          NOTE! if NULL may cause memory leak
 * @udt     opaque pram pass to callback
 * @return  0 sucess, M_Exxx otherwise
 * @sample  void cbk_free(void *elem, void *udt)
 *          {
 *              struct element *em = (struct element *)elem;
 *              void *userdata = udt;
 *              free(em);
 *          }
 *          m_rbtree_free(tree, cbk_free, userdata);
********************************************************/
int m_rbtree_free(struct m_rbtree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt);

/*******************************************************
 * @brief   insert an new element into rbtree
 * @tree    rbtree instance addr
 * @elem    the new element, key of element must unique, can not eque 
 *          than other exist element in rbtree, otherwise insert will
 *          failure and return M_EEXSTS
 * @cbk     callback function use for compare element key
 *          @ielem  rbtree internal element
 *          @elem   the new element
 *          @udt    opaque data
 *          @return 1 if key of ielem greater than elem
 *                  -1 if key of ielem less than elem
 *                  0 if key of ielem eque elem
 * @udt     opaque pram will pass to callback
 * @return  0 sucess, M_Exxx otherwise
 * @sample  void cbk_insert(void *ielem, void *elem, void *udt)
 *          {
 *              if (ielem->key > elem->key)
 *                  return 1;
 *              else if (ielem->key < elem->key)
 *                  return -1;
 *              else
 *                  return 0;
 *          }
 *          m_rbtree_insert(tree, elem, cbk_insert, userdata);
********************************************************/
int m_rbtree_insert(struct m_rbtree *tree, void *elem,
                    int (*cbk)(void *ielem, void *elem, void *udt), void *udt);

/*******************************************************
 * @brief   remove an element from rbtree (is not free element memory)
 * @tree    rbtree instance addr
 * @elem    the element will remove, must in rbtree
 * @return  0 sucess, M_Exxx otherwise
********************************************************/
int m_rbtree_remove(struct m_rbtree *tree, void *elem);

/*******************************************************
 * @brief   find an element from rbtree by given key
 * @tree    rbtree instance addr
 * @key     the key of element will to find
 * @cbk     callback function use for compare element key
 *          @ielem  rbtree internal element
 *          @key    the key
 *          @udt    opaque data
 *          @return 1 if key of ielem greater than key
 *                  -1 if key of ielem less than key
 *                  0 if key of ielem eque key
 * @udt     opaque pram will pass to callback
 * @return  found element, NULL otherwise
 * @sample  void cbk_find(void *ielem, void *key, void *udt)
 *          {
 *              if (ielem->key > key)
 *                  return 1;
 *              else if (ielem->key < key)
 *                  return -1;
 *              else
 *                  return 0;
 *          }
 *          m_rbtree_find(tree, key, cbk_find, userdata);
********************************************************/
void *m_rbtree_find(struct m_rbtree *tree, void *key,
                int (*cbk)(void *ielem, void *key, void *udt), void *udt);

/*******************************************************
 * @brief   find prev element of given element in rbtree
 * @tree    rbtree instance addr
 * @elem    given element
 * @return  found element, NULL otherwise
********************************************************/
void *m_rbtree_prev(struct m_rbtree *tree, void *elem);

/*******************************************************
 * @brief   find next element of given element in rbtree
 * @tree    rbtree instance addr
 * @elem    given element
 * @return  found element, NULL otherwise
********************************************************/
void *m_rbtree_next(struct m_rbtree *tree, void *elem);

/*******************************************************
 * @brief   find first element in rbtree (leftmost node)
 * @tree    rbtree instance addr
 * @return  found element, NULL otherwise
********************************************************/
void *m_rbtree_first(struct m_rbtree *tree);

/*******************************************************
 * @brief   find last element in rbtree (rightmost node)
 * @tree    rbtree instance addr
 * @return  found element, NULL otherwise
********************************************************/
void *m_rbtree_last(struct m_rbtree *tree);

/*******************************************************
 * @brief   find root element in rbtree
 * @tree    rbtree instance addr
 * @return  found element, NULL otherwise
********************************************************/
void *m_rbtree_root(struct m_rbtree *tree);

/*******************************************************
 * @brief   orderly traversal rbtree
 * @tree    rbtree instance addr
 * @cbk     callback function use for return every element
 * @sample  void cbk(void *elem, void *udt)
 *          {
 *              printf("elem->key", elem->key);
 *          }
 *          m_rbtree_inoder(tree, cbk, userdata);
********************************************************/
void m_rbtree_inorder(struct m_rbtree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt);
void m_rbtree_preorder(struct m_rbtree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt);
void m_rbtree_postorder(struct m_rbtree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt);

/*******************************************************
 * @brief   judge an rbtree is balance or not
 * @tree    rbtree instance addr
 * @return  0 balance, otherwise not
********************************************************/
int m_rbtree_judge(struct m_rbtree *tree);

#ifdef __cplusplus
}
#endif

#endif
