

#include "rbtree.h"

#ifndef inline
#define inline __inline
#endif

#define RB_RED      0
#define RB_BLACK    1

/********************************************************
 * @brief   get rbnode from element
 * @DATA    element instance addr
 * @OFFSET  rbtree.offset
 * @sample  struct m_rbnode *node = ELEM2NODE(elem, rbtree.offset);
*********************************************************/
#define ELEM2NODE(ELEM,OFFSET) ((struct m_rbnode *)((size_t)(ELEM) + (OFFSET)))

/********************************************************
 * @brief   get element from rbnode
 * @NODE    rbnode
 * @OFFSET  rbtree.offset
 * @sample  struct element *elem = NODE2ELEM(rbnode, rbtree.offset);
*********************************************************/
#define NODE2ELEM(NODE,OFFSET) ((void *)((size_t)(NODE) - (OFFSET)))

static inline void left_rotate(struct m_rbnode *node, struct m_rbnode **root)
{
    struct m_rbnode *right = node->right;
    
    if ((node->right = right->left))
        right->left->parent = node;
    right->left = node;
    
    if ((right->parent = node->parent)) {
        if (node->parent->left == node)
            node->parent->left = right;
        else
            node->parent->right = right;
    } else {
        *root = right;
    }
    node->parent = right;
}

static inline void right_rotate(struct m_rbnode *node, struct m_rbnode **root)
{
    struct m_rbnode *left = node->left;
    
    if ((node->left = left->right))
        left->right->parent = node;
    left->right = node;
    
    if ((left->parent = node->parent)) {
        if (node->parent->left == node)
            node->parent->left = left;
        else
            node->parent->right = left;
    } else {
        *root = left;
    }
    node->parent = left;
}

static void rbnode_insert_colour(struct m_rbnode *node,
                    struct m_rbnode **root)
{
    struct m_rbnode *uncle = NULL;
    struct m_rbnode *parent = NULL;
    struct m_rbnode *gparent = NULL;
    
    while ((parent = node->parent) && parent->color == RB_RED) {
        gparent = parent->parent;
        if (parent == gparent->left) {
            uncle = gparent->right;
            if (uncle && uncle->color == RB_RED) {
                uncle->color = RB_BLACK;
                parent->color = RB_BLACK;
                gparent->color = RB_RED;
                node = gparent;
                continue;
            }

            if (parent->right == node) {
				register struct m_rbnode *tmp;
				left_rotate(parent, root);
				tmp = parent;
				parent = node;
				node = tmp;
			}
    
            parent->color = RB_BLACK;
            gparent->color = RB_RED;
            right_rotate(gparent, root);
        } else {
            uncle = gparent->left;
            if (uncle && uncle->color == RB_RED) {
                uncle->color = RB_BLACK;
                parent->color = RB_BLACK;
                gparent->color = RB_RED;
                node = gparent;
                continue;
            }

            if (parent->left == node) {
				register struct m_rbnode *tmp;
				right_rotate(parent, root);
				tmp = parent;
				parent = node;
				node = tmp;
			}
    
            parent->color = RB_BLACK;
            gparent->color = RB_RED;
            left_rotate(gparent, root);
        }
    }

    (*root)->color = RB_BLACK;
}

static void rbnode_remove_colour(struct m_rbnode *node,
                    struct m_rbnode *parent, struct m_rbnode **root)
{
    struct m_rbnode *other = NULL;

	while ((!node || node->color == RB_BLACK) && node != *root) {
		if (parent->left == node) {
			other = parent->right;
			if (other->color == RB_RED) {
				other->color = RB_BLACK;
				parent->color = RB_RED;
				left_rotate(parent, root);
				other = parent->right;
			}
			if ((!other->left || other->left->color == RB_BLACK)
			            && (!other->right || other->right->color == RB_BLACK)){
				other->color = RB_RED;
				node = parent;
				parent = node->parent;
			} else {
				if (!other->right || other->right->color == RB_BLACK) {
					register struct m_rbnode *o_left;
					if ((o_left = other->left))
						o_left->color = RB_BLACK;
					other->color = RB_RED;
					right_rotate(other, root);
					other = parent->right;
				}
				other->color = parent->color;
				parent->color = RB_BLACK;
				if (other->right)
					other->right->color = RB_BLACK;
				left_rotate(parent, root);
				node = *root;
				break;
			}
		} else {
			other = parent->left;
			if (other->color == RB_RED) {
				other->color = RB_BLACK;
				parent->color = RB_RED;
				right_rotate(parent, root);
				other = parent->left;
			}
			if ((!other->left || other->left->color == RB_BLACK)
			            && (!other->right || other->right->color == RB_BLACK)){
				other->color = RB_RED;
				node = parent;
				parent = node->parent;
			} else {
				if (!other->left || other->left->color == RB_BLACK) {
					register struct m_rbnode *o_right;
					if ((o_right = other->right))
						o_right->color = RB_BLACK;
					other->color = RB_RED;
					left_rotate(other, root);
					other = parent->left;
				}
				other->color = parent->color;
				parent->color = RB_BLACK;
				if (other->left)
					other->left->color = RB_BLACK;
				right_rotate(parent, root);
				node = *root;
				break;
			}
		}
	}
	if (node)
		node->color = RB_BLACK;
}

int m_rbtree_init(struct m_rbtree *tree, size_t offset)
{
    if (!tree) return M_EINVAL;

    tree->root = NULL;
    tree->offset = offset;
    tree->count = 0;

    return 0;
}

int m_rbtree_free(struct m_rbtree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!tree) return M_EINVAL;

    struct m_rbnode *node = tree->root;
    while (node) {
        /* find one leaf */
        while (1) {
            if (node->left)
                node = node->left;
            else if (node->right)
                node = node->right;
            else
                break;
        }

        if (node->parent == NULL) {
            if (cbk) cbk(NODE2ELEM(node, tree->offset), udt);
            tree->root = NULL;
            tree->offset = 0;
            tree->count = 0;
            return 0;
        } else {
            struct m_rbnode *parent = node->parent;
            
            if (parent->left == node)
                parent->left = NULL;
            else
                parent->right = NULL;
        
            if (cbk) cbk(NODE2ELEM(node, tree->offset), udt);
            node = parent;
        }
    }

    return 0;
}

int m_rbtree_insert(struct m_rbtree *tree, void *elem,
                    int (*cbk)(void *ielem, void *elem, void *udt), void *udt)
{
    if (!tree || !elem || !cbk) return M_EINVAL;

    struct m_rbnode **link = &tree->root;
    struct m_rbnode *parent = NULL;
    struct m_rbnode *node = ELEM2NODE(elem,tree->offset);
    
    while (*link) {
        int ret = 0;
        parent = *link;
        ret = cbk(NODE2ELEM(parent, tree->offset), elem, udt);
        if (ret > 0)
            link = &parent->left;
        else if (ret < 0)
            link = &parent->right;
        else
            return M_EEXISTS;
    }
    node->parent = parent;
    node->color = RB_RED;
    node->left = node->right = NULL;
    *link = node;
    
    rbnode_insert_colour(node, &tree->root);
    tree->count++;
    
    return 0;
}

int m_rbtree_remove(struct m_rbtree *tree, void *elem)
{
    int color = 0;
    struct m_rbnode *node = NULL;
    struct m_rbnode *child = NULL;
    struct m_rbnode *parent = NULL;
    if (!tree || !elem) return M_EINVAL;

    node = ELEM2NODE(elem, tree->offset);
    if (!node->left) {
        child = node->right;
    } else if (!node->right) {
        child = node->left;
    } else {
        struct m_rbnode *old = node;
        /* find smallest node of the right */
        node = node->right;
        while (node->left)
            node = node->left;
        child = node->right;
        parent = node->parent;
        color = node->color;

        if (child)
            child->parent = node->parent;
        if (parent) {
            if (parent->left == node)
                parent->left = child;
            else
                parent->right = child;
        } else {
            tree->root = child;
        }

        if (parent == old)
            parent = node;
        node->parent = old->parent;
        node->left = old->left;
        node->right = old->right;
        node->color = old->color;

        if (old->parent) {
            if (old->parent->left == old)
                old->parent->left = node;
            else
                old->parent->right = node;
        } else {
            tree->root = node;
        }

        old->left->parent = node;
        if (old->right)
            old->right->parent = node;

        goto colour;
    }

    parent = node->parent;
    color = node->color;

    if (child)
        child->parent = parent;
    if (parent) {
        if (parent->left == node)
            parent->left = child;
        else
            parent->right = child;
    } else {
        tree->root = child;
    }

colour:
    if (color == RB_BLACK)
        rbnode_remove_colour(child, parent, &tree->root);

    return 0;
}

void *m_rbtree_find(struct m_rbtree *tree, void *key,
                int (*cbk)(void *ielem, void *key, void *udt), void *udt)
{
    if (!tree || !cbk) return NULL;

    struct m_rbnode *node = tree->root;
    while (node) {
        int ret = cbk(NODE2ELEM(node,tree->offset), key, udt);
        if (ret > 0)
            node = node->left;
        else if (ret < 0)
            node = node->right;
        else
            return NODE2ELEM(node,tree->offset);
    }
        
    return NULL;
}

void *m_rbtree_first(struct m_rbtree *tree)
{
    if (!tree) return NULL;

    struct m_rbnode *node = tree->root;
    if (node) {
        while (node->left)
            node = node->left;
        return NODE2ELEM(node,tree->offset);
    }

    return NULL;
}

void *m_rbtree_last(struct m_rbtree *tree)
{
    if (!tree) return NULL;

    struct m_rbnode *node = tree->root;
    if (node) {
        while (node->right)
            node = node->right;
        return NODE2ELEM(node,tree->offset);
    }

    return NULL;
}

void *m_rbtree_prev(struct m_rbtree *tree, void *elem)
{
    if (!tree) return NULL;

    struct m_rbnode *node = ELEM2NODE(elem,tree->offset);
	if (node->left) {
		node = node->left;
		while (node->right) 
			node = node->right;
	} else {
		while (1) {
			struct m_rbnode *last = node;
			node = node->parent;
			if (node == NULL)
			    return NULL;
			if (node->right == last)
			    break;
		}
	}
	
	return NODE2ELEM(node,tree->offset);
}

void *m_rbtree_next(struct m_rbtree *tree, void *elem)
{
    if (!tree) return NULL;
    
    struct m_rbnode *node = ELEM2NODE(elem,tree->offset);
	if (node->right) {
		node = node->right;
		while (node->left) 
			node = node->left;
	} else {
		while (1) {
			struct m_rbnode *last = node;
			node = node->parent;
			if (node == NULL)
			    return NULL;
			if (node->left == last)
			    break;
		}
	}
	
	return NODE2ELEM(node,tree->offset);
}

void *m_rbtree_root(struct m_rbtree *tree)
{
    if (!tree) return NULL;
    if (!tree->root) return NULL;

    return NODE2ELEM(tree->root,tree->offset);
}

static void rbtree_inorder(size_t offset, struct m_rbnode *node,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!node) return;
    rbtree_inorder(offset, node->left, cbk, udt);
    cbk(NODE2ELEM(node,offset), udt);
    rbtree_inorder(offset, node->right, cbk, udt);
}

void m_rbtree_inorder(struct m_rbtree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!tree || !cbk) return;

    return rbtree_inorder(tree->offset, tree->root, cbk, udt);
}

static void rbree_preorder(size_t offset, struct m_rbnode *node,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!node) return;
    cbk(NODE2ELEM(node,offset), udt);
    rbree_preorder(offset, node->left, cbk, udt);
    rbree_preorder(offset, node->right, cbk, udt);
}

void m_rbtree_preorder(struct m_rbtree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!tree || !cbk) return;

    return rbree_preorder(tree->offset, tree->root, cbk, udt);
}

static void rbtree_postorder(size_t offset, struct m_rbnode *node,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!node) return;
    rbtree_postorder(offset, node->left, cbk, udt);
    rbtree_postorder(offset, node->right, cbk, udt);
    cbk(NODE2ELEM(node,offset), udt);
}

void m_rbtree_postorder(struct m_rbtree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!tree || !cbk) return;

    return rbtree_postorder(tree->offset, tree->root, cbk, udt);
}

static int rbtree_judge(struct m_rbnode *node, int *result)
{
    int blacknum_l=0;
    int blacknum_r=0;
    if(node == NULL) return -1;
    
    *result = 0;
    
    if(node->color != RB_BLACK && node->color != RB_RED) {
        *result=-1;
        return -1;
    }
    if(node->parent == NULL) {
        if(node->color != RB_BLACK) {
            *result=-1;
            return -1;
        }
    } else {
        if(node->color == RB_RED && node->parent->color == RB_RED) {
            *result=-1;
            return -1;
        }
    }
    
    if(*result == 0)
        blacknum_l += rbtree_judge(node->left, result);
    if(*result == 0)
        blacknum_r += rbtree_judge(node->right, result);
    if(*result != 0)
        return -1;
        
    if(node->left && node->left->color == RB_BLACK)
    {
        ++blacknum_l;
    }
    if(node->right && node->right->color == RB_BLACK)
    {
        ++blacknum_r;
    }
    
    if(blacknum_l != blacknum_r)
    {
        *result=-1;
        return -1;
    }
    
    return blacknum_l;
}

int m_rbtree_judge(struct m_rbtree *tree)
{
    int result = -1;
    if (!tree) return M_EINVAL;

    rbtree_judge(tree->root, &result);

    return result;
}
