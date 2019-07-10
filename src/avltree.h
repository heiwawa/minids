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
#define M_EEXISTS   (-4) /* equal key of element already exist */
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
    unsigned int height; /* (max height of childs) + 1 */
};

struct m_avltree {
    struct m_avlnode *root; /* tree root node */
    size_t offset;          /* offset of avlnode in element */
    size_t count;           /* node count of tree */
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
 * @cbk     callback function use for free element memory
 *          NOTE! if NULL may cause memory leak
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

/*******************************************************
 * @brief   insert an new element into avltree
 * @tree    avltree instance addr
 * @elem    the new element, key of element must unique, can not eque 
 *          than other exist element in avltree, otherwise insert will
 *          failure and return M_EEXSTS
 * @cbk     callback function use for compare element key
 *          @ielem  avltree internal element
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
 *          m_avltree_insert(tree, elem, cbk_insert, userdata);
********************************************************/
int m_avltree_insert(struct m_avltree *tree, void *elem,
                int (*cbk)(void *ielem, void *elem, void *udt), void *udt);

/*******************************************************
 * @brief   remove an element from avltree (is not free element memory)
 * @tree    avltree instance addr
 * @elem    the element will remove, must in avltree
 * @return  0 sucess, M_Exxx otherwise
********************************************************/
int m_avltree_remove(struct m_avltree *tree, void *elem);

/*******************************************************
 * @brief   find an element from avltree
 * @tree    avltree instance addr
 * @key     the key of element will to find
 * @cbk     callback function use for compare element key
 *          @ielem  avltree internal element
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
 *          m_avltree_find(tree, key, cbk_find, userdata);
********************************************************/
void *m_avltree_find(struct m_avltree *tree, void *key,
                int (*cbk)(void *ielem, void *key, void *udt), void *udt);

/*******************************************************
 * @brief   find prev element of given element in avltree
 * @tree    avltree instance addr
 * @elem    given element
 * @return  found element, NULL otherwise
********************************************************/
void *m_avltree_prev(struct m_avltree *tree, void *elem);

/*******************************************************
 * @brief   find next element of given element in avltree
 * @tree    avltree instance addr
 * @elem    given element
 * @return  found element, NULL otherwise
********************************************************/
void *m_avltree_next(struct m_avltree *tree, void *elem);

/*******************************************************
 * @brief   find first element in avltree (leftmost node)
 * @tree    avltree instance addr
 * @return  found element, NULL otherwise
********************************************************/
void *m_avltree_first(struct m_avltree *tree);

/*******************************************************
 * @brief   find last element in avltree (rightmost node)
 * @tree    avltree instance addr
 * @return  found element, NULL otherwise
********************************************************/
void *m_avltree_last(struct m_avltree *tree);

/*******************************************************
 * @brief   find root element in avltree
 * @tree    avltree instance addr
 * @return  found element, NULL otherwise
********************************************************/
void *m_avltree_root(struct m_avltree *tree);


/*******************************************************
 * @brief   judge an avltree is balance or not
 * @tree    avltree instance addr
 * @return  0 balance, otherwise not
********************************************************/
int m_avltree_judge(struct m_avltree *tree);

/*******************************************************
 * @brief   orderly traversal avltree
 * @tree    avltree instance addr
 * @cbk     callback function use for return every element
 * @sample  void cbk(void *elem, void *udt)
 *          {
 *              printf("elem->key", elem->key);
 *          }
 *          m_avltree_inoder(tree, cbk, userdata);
********************************************************/
void m_avltree_inorder(struct m_avltree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt);
void m_avltree_preorder(struct m_avltree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt);
void m_avltree_postorder(struct m_avltree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt);

#ifdef __cplusplus
}
#endif

#endif

