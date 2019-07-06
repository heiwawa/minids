


#include "avltree.h"

/********************************************************
 * @brief   get avlnode from element
 * @DATA    element instance addr
 * @OFFSET  avltree.offset
 * @sample  struct m_avlnode *node = ELEM2NODE(elem, avltree.offset);
*********************************************************/
#define ELEM2NODE(ELEM,OFFSET) ((struct m_avlnode *)((size_t)(ELEM)+(OFFSET)))

/********************************************************
 * @brief   get element from avlnode
 * @NODE    avlnode
 * @OFFSET  avltree.offset
 * @sample  struct element *elem = NODE2ELEM(avlnode, avltree.offset);
*********************************************************/
#define NODE2ELEM(NODE,OFFSET) ((void *)((size_t)(NODE) - (OFFSET)))

int m_avltree_init(struct m_avltree *tree, size_t offset)
{
    if (!tree) return M_EINVAL;

    tree->root = NULL;
    tree->offset = offset;
    tree->count = 0;

    return 0;
}

int m_avltree_free(struct m_avltree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!tree || !cbk) return M_EINVAL;

    struct m_avlnode *parent = NULL;
    struct m_avlnode *node = tree->root;
    
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

        parent = node->parent;
        if (parent) {
            /* cut off with parent */
            if (parent->left == node)
                parent->left = NULL;
            else if (parent->right == node)
                parent->right = NULL;
            else
                return M_EUNKNOWN;
        } else {
            /* if parent is NULL, then it is root node */
            tree->root = NULL;
            tree->offset = 0;
            tree->count = 0;
        }

        /* call callback function to free element */
        /* node->left = node->right = node->parent = node->height = 0; */
        cbk(NODE2ELEM(node,tree->offset), udt);

        /* upward to free */
        node = parent;        
    }

    return 0;
}

#if 0
struct _sl_avltree {
    void *elem;
    struct _sl_avltree *left;
    struct _sl_avltree *right;
    struct _sl_avltree *parent;    /* pointing to node itself for empty node */
    int height;                 /* equals to 1 + max height in childs */
};

#define replace_node(old,new,parent,root) \
    do { \
        if (parent) { \
            if (parent->left == old) \
                parent->left = new; \
            else \
                parent->right = new; \
        } else { \
            *root = new; \
        } \
    } while (0)

#define left_height(node) (((node)->left)? ((node)->left)->height : 0)

#define right_height(node) (((node)->right)? ((node)->right)->height : 0)

#define reset_height(node) \
	    (node->height = SL_MAX(left_height(node), right_height(node)) + 1)

static inline sl_avltree node_alloc(sl_avltree parent, void *elem)
{
    sl_avltree node = (sl_avltree)sl_malloc(sizeof(*node), NULL);
    node->elem = elem;
    node->parent = parent;
    node->height = 0;
    node->left = node->right = NULL;

    return node;
}

static inline sl_avltree left_rotation(sl_avltree node, sl_avltree *root)
{
    sl_avltree right = node->right;
	sl_avltree parent = node->parent;
	node->right = right->left;
	if (right->left) 
		right->left->parent = node;
	right->left = node;
	right->parent = parent;
	replace_node(node, right, parent, root);
	node->parent = right;
	
	return right;
}

static inline sl_avltree right_rotation(sl_avltree node, sl_avltree *root)
{
    sl_avltree left = node->left;
	sl_avltree parent = node->parent;
	node->left = left->right;
	if (left->right) 
		left->right->parent = node;
	left->right = node;
	left->parent = parent;
	replace_node(node, left, parent, root);
	node->parent = left;
	
	return left;
}

static inline sl_avltree rotate_right(sl_avltree node, sl_avltree *root)
{
    sl_avltree right = node->right;
	
	if (left_height(right) > right_height(right)) {
		right = right_rotation(right, root);
		reset_height(right->right);
		reset_height(right);
	}
	
	node = left_rotation(node, root);
	reset_height(node->left);
	reset_height(node);
	
	return node;
}

static inline sl_avltree rotate_left(sl_avltree node, sl_avltree *root)
{
    sl_avltree left = node->left;
	
	if (right_height(left) > left_height(left)) {
		left = left_rotation(left, root);
		reset_height(left->left);
		reset_height(left);
	}
	
	node = right_rotation(node, root);
	reset_height(node->right);
	reset_height(node);
	
	return node;
}

static void insert_rebalance(sl_avltree node, sl_avltree *root)
{
    node->height = 1;
    
    for (node = node->parent; node; node = node->parent) {
		int h0 = (int)left_height(node);
		int h1 = (int)right_height(node);
		int height = SL_MAX(h0, h1) + 1;
		int diff = h0 - h1;
		
		if (node->height == height)
		    break;
		node->height = height;

		if (diff <= -2) {
			node = rotate_right(node, root);
		}
		else if (diff >= 2) {
			node = rotate_left(node, root);
		}
	}
}

static void remove_rebalance(sl_avltree node, sl_avltree *root)
{
    while (node) {
		int h0 = (int)left_height(node);
		int h1 = (int)right_height(node);
		int diff = h0 - h1;
		int height = SL_MAX(h0, h1) + 1;

		/* height not change and height diff between -1 and 1 */
		if (node->height != height) {
			node->height = height;
		} else if (diff >= -1 && diff <= 1) {
			break;
		}

        /* rebalance */
		if (diff <= -2) {
			node = rotate_right(node, root);
		}
		else if (diff >= 2) {
			node = rotate_left(node, root);
		}
		node = node->parent;
	}
}

static void node_remove(sl_avltree node, sl_avltree *root)
{
    sl_avltree child = NULL;
    sl_avltree parent = NULL;
    
	if (node->left && node->right) {
		sl_avltree old = node;
		sl_avltree left = NULL;

		/* find smallest child of right tree */
		node = node->right;
		while ((left = node->left) != NULL)
			node = left;

        /* replace smallest child of right tree to its right child */
		child = node->right;
		parent = node->parent;
		if (child)
			child->parent = parent;
		replace_node(node, child, parent, root);

		/* put smallest child of right tree to remove position */
		if (node->parent == old)
			parent = node;
		node->left = old->left;
		node->right = old->right;
		node->parent = old->parent;
		node->height = old->height;
		replace_node(old, node, old->parent, root);
		old->left->parent = node;
		if (old->right) {
			old->right->parent = node;
		}
	} else {
		if (node->left == NULL) 
			child = node->right;
		else
			child = node->left;
		parent = node->parent;
		replace_node(node, child, parent, root);
		if (child) {
			child->parent = parent;
		}
	}
	
	if (parent) {
		remove_rebalance(parent, root);
	}
}

int sl_avltree_insert(sl_avltree *pavl, void *elem,
                int (*compare)(void *ielem, void *elem, void *udt), void *udt)
{
    sl_assert_return_val(pavl != NULL,SL_EINVAL,"args 'pavl' is NULL");
    sl_assert_return_val(compare != NULL,SL_EINVAL,"args 'compare' is NULL");
    sl_avltree *pnode = pavl;
    sl_avltree parent = NULL;

    /* insert first node */
    if (*pavl == NULL) {
        *pavl = node_alloc(NULL, elem);
        
        return SL_SUCCESS;
    }

    while (*pnode) {
        int ret = compare((*pnode)->elem, elem, udt);
        parent = *pnode;
        if (ret > 0)
            pnode = &parent->left;
        else if (ret < 0)
            pnode = &parent->right;
        else
            return SL_EEXISTS;
    }
    
    *pnode = node_alloc(parent, elem);
    insert_rebalance(*pnode, pavl);

    return SL_SUCCESS;
}

int sl_avltree_remove(sl_avltree *pavl, void *key,
                int (*compare)(void *ielem, void *key, void *udt), void *udt)
{
    sl_assert_return_val(pavl != NULL,SL_EINVAL,"args 'pavl' is NULL");
    sl_assert_return_val(compare != NULL,SL_EINVAL,"args 'compare' is NULL");

    sl_avltree node = *pavl;
    while (node) {
        int ret = compare(node->elem, key, udt);
        if (ret > 0)
            node = node->left;
        else if (ret < 0)
            node = node->right;
        else
            break;
    }
    if (node) {
        node_remove(node, pavl);
        sl_free(node);
    } else {
        return SL_ENOTFOUND;
    }

    return SL_SUCCESS;
}

void *sl_avltree_find(sl_avltree avl, void *key,
                int (*compare)(void *ielem, void *key, void *udt), void *udt)
{
    sl_assert_return_val(compare != NULL,NULL,"args 'compare' is NULL");

    sl_avltree node = avl;
    while (node) {
        int ret = compare(node->elem, key, udt);
        if (ret > 0)
            node = node->left;
        else if (ret < 0)
            node = node->right;
        else
            return node->elem;
    }
    
    return NULL;
}

void *sl_avltree_prev(sl_avltree avl, void *key,
                int (*compare)(void *ielem, void *key, void *udt), void *udt)
{
    sl_assert_return_val(compare != NULL,NULL,"args 'compare' is NULL");

    /* find node by key */
    sl_avltree node = avl;
    while (node) {
        int ret = compare(node->elem, key, udt);
        if (ret > 0)
            node = node->left;
        else if (ret < 0)
            node = node->right;
        else
            break;
    }
    if (node == NULL)
        return NULL;
        
	if (node->left) {
		node = node->left;
		while (node->right) 
			node = node->right;
	} else {
		while (1) {
			sl_avltree last = node;
			node = node->parent;
			if (node == NULL)
			    return NULL;
			if (node->right == last)
			    break;
		}
	}
	
	return node->elem;
}

void *sl_avltree_next(sl_avltree avl, void *key,
                int (*compare)(void *ielem, void *key, void *udt), void *udt)
{
    sl_assert_return_val(compare != NULL,NULL,"args 'compare' is NULL");

    /* find node by key */
    sl_avltree node = avl;
    while (node) {
        int ret = compare(node->elem, key, udt);
        if (ret > 0)
            node = node->left;
        else if (ret < 0)
            node = node->right;
        else
            break;
    }
    if (node == NULL)
        return NULL;
        
	if (node->right) {
		node = node->right;
		while (node->left) 
			node = node->left;
	} else {
		while (1) {
			sl_avltree last = node;
			node = node->parent;
			if (node == NULL)
			    return NULL;
			if (node->left == last)
			    break;
		}
	}
	
	return node->elem;
}

void *sl_avltree_first(sl_avltree avl)
{
    sl_avltree node = avl;
    if (!node)
        return NULL;
        
    while (node->left)
        node = node->left;

    return node->elem;
}

void *sl_avltree_last(sl_avltree avl)
{
    sl_avltree node = avl;
    if (!node)
        return NULL;
        
    while (node->right)
        node = node->right;

    return node->elem;
}

void sl_avltree_free(sl_avltree *pavl,
                    void (*free)(void *elem, void *udt), void *udt)
{
    if (!pavl)
        return;
    sl_avltree node = *pavl;
    sl_avltree tmp = NULL;

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
            if (free) free(node->elem, udt);
            sl_free(node);
            *pavl = NULL;
            return;
        } else {
            if (node->parent->left == node)
                node->parent->left = NULL;
            else
                node->parent->right = NULL;
        
            tmp = node->parent;
            sl_free(node);
            node = tmp;
        }
    }
}

int sl_avltree_judge(sl_avltree avl, int *height)
{
    if (!avl) {
        *height = 0;
        return 1;
    }
    
    int lheight;  
    int lresult = sl_avltree_judge(avl->left, &lheight);  
    int rheight;  
    int rresult = sl_avltree_judge(avl->right, &rheight);

    *height = SL_MAX(lheight, rheight) + 1;
    if(lresult && rresult && SL_ABS(lheight - rheight) < 2)
        return 1;  
    else
        return 0;  
}

#ifdef __SLLIB_H__
void sl_avltree_print(sl_avltree avl,
                    void (*func)(int newline, void *data))
{
    int current = 0;
    int next = 0;
    sl_queue queue = NULL;
    sl_assert_return(avl != NULL || func != NULL,"invalid arguments");

    queue = sl_queue_create();

    sl_queue_push_tail(queue, avl);
    current = 1;
    next = 0;
    while (sl_queue_is_empty(queue) == SL_FALSE) {
        sl_avltree node = sl_queue_pop_head(queue);
        func(0, node->elem);
        current--;

        if (node->left) {
            sl_queue_push_tail(queue, node->left);
            next++;
        }
        if (node->right) {
            sl_queue_push_tail(queue, node->right);
            next++;
        }

        if (current == 0) {
            func(1, NULL);
            current = next;
            next = 0;
        }
    }
}
#endif
#endif
