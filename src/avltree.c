


#include "avltree.h"

#ifndef inline
#define inline __inline
#endif

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

/* left child height */
#define LEFT_HEIGHT(node) (((node)->left)? ((node)->left)->height : 0)
/* right child height */
#define RIGHT_HEIGHT(node) (((node)->right)? ((node)->right)->height : 0)
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define ABS(a) (((a) < 0) ? -(a) : (a))
#define HEIGHT_RESET(node) \
            (node->height = MAX(LEFT_HEIGHT(node),RIGHT_HEIGHT(node)) + 1)

#define PARENT_RESET(root,parent,node,newnode) \
    do { \
        if (parent) { \
            if (parent->left == node) \
                parent->left = newnode; \
            else \
                parent->right = newnode; \
        } else { \
            *root = newnode; \
        } \
    } while (0)

static inline struct m_avlnode * left_rotate(struct m_avlnode *node,
                    struct m_avlnode **root)
{
    struct m_avlnode *right = node->right;
	struct m_avlnode *parent = node->parent;
    
	node->right = right->left;
	if (node->right) node->right->parent = node;
    node->parent = right;
	right->left = node;
	right->parent = parent;
	PARENT_RESET(root, parent, node, right);
	
	
	return right;
}

static inline struct m_avlnode * right_rotate(struct m_avlnode * node,
                    struct m_avlnode **root)
{
    struct m_avlnode *left = node->left;
	struct m_avlnode *parent = node->parent;
    
	node->left = left->right;
	if (node->left) node->left->parent = node;
    node->parent = left;
	left->right = node;
	left->parent = parent;
	PARENT_RESET(root, parent, node, left);
	
	return left;
}

static inline struct m_avlnode * right_left_rotate(
                    struct m_avlnode *node, struct m_avlnode **root)
{
    struct m_avlnode *right = node->right;
	
	if (LEFT_HEIGHT(right) > RIGHT_HEIGHT(right)) {
		right = right_rotate(right, root);
		HEIGHT_RESET(right->right);
		HEIGHT_RESET(right);
	}
	
	node = left_rotate(node, root);
	HEIGHT_RESET(node->left);
	HEIGHT_RESET(node);
	
	return node;
}

static inline struct m_avlnode * left_right_rotate(
                    struct m_avlnode *node, struct m_avlnode **root)
{
    struct m_avlnode *left = node->left;
	
	if (RIGHT_HEIGHT(left) > LEFT_HEIGHT(left)) {
		left = left_rotate(left, root);
		HEIGHT_RESET(left->left);
		HEIGHT_RESET(left);
	}
	
	node = right_rotate(node, root);
	HEIGHT_RESET(node->right);
	HEIGHT_RESET(node);
	
	return node;
}

static void insert_rebalance(struct m_avlnode *node,
                    struct m_avlnode **root)
{
    node->height = 1;
    
    for (node = node->parent; node; node = node->parent) {
        int diff = (int)LEFT_HEIGHT(node) - (int)RIGHT_HEIGHT(node);
		int height = MAX(LEFT_HEIGHT(node), RIGHT_HEIGHT(node)) + 1;
		
		if (node->height == height)
		    break;
		node->height = height;

		if (diff <= -2)
			node = right_left_rotate(node, root);
		else if (diff >= 2)
			node = left_right_rotate(node, root);
	}
}

static void remove_rebalance(struct m_avlnode *node,
                    struct m_avlnode **root)
{
    while (node) {
		int diff = (int)LEFT_HEIGHT(node) - (int)RIGHT_HEIGHT(node);
		int height = MAX(LEFT_HEIGHT(node), RIGHT_HEIGHT(node)) + 1;

		/* height not change and height diff between -1 and 1 */
		if (node->height != height)
			node->height = height;
		else if (diff >= -1 && diff <= 1)
			break;

        /* rebalance */
		if (diff <= -2)
			node = right_left_rotate(node, root);
		else if (diff >= 2)
			node = left_right_rotate(node, root);
        
		node = node->parent;
	}
}

static void node_remove(struct m_avlnode *node, struct m_avlnode **root)
{
    struct m_avlnode *child = NULL;
    struct m_avlnode *parent = NULL;
    
	if (node->left && node->right) {
		struct m_avlnode *old = node;
		struct m_avlnode *left = NULL;

		/* find smallest child of right tree */
		node = node->right;
		while ((left = node->left) != NULL)
			node = left;

        /* replace smallest child of right tree to its right child */
		child = node->right;
		parent = node->parent;
		if (child)
			child->parent = parent;
        PARENT_RESET(root,parent,node,child);

		/* put smallest child of right tree to remove position */
		if (node->parent == old)
			parent = node;
		node->left = old->left;
		node->right = old->right;
		node->parent = old->parent;
		node->height = old->height;
        PARENT_RESET(root,old->parent,old,node);
		old->left->parent = node;
		if (old->right)
			old->right->parent = node;
	} else {
		if (node->left) 
			child = node->left;
		else
			child = node->right;
		parent = node->parent;
        PARENT_RESET(root,parent,node,child);
		if (child)
			child->parent = parent;
	}
	
	if (parent)
		remove_rebalance(parent, root);
}

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
    if (!tree) return M_EINVAL;

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
        if (!parent) {
            /* if parent is NULL, then it is root node */
            if (cbk) cbk(NODE2ELEM(node,tree->offset), udt);
            tree->root = NULL;
            tree->offset = 0;
            tree->count = 0;
            return 0;
        }
        
        /* cut off with parent */
        if (parent->left == node)
            parent->left = NULL;
        else if (parent->right == node)
            parent->right = NULL;
        else
            return M_EUNKNOWN;

        /* call callback function to free element */
        /* node->left = node->right = node->parent = node->height = 0; */
        if (cbk) cbk(NODE2ELEM(node,tree->offset), udt);

        /* upward to free */
        node = parent;        
    }

    return 0;
}

int m_avltree_insert(struct m_avltree *tree, void *elem,
                    int (*cbk)(void *ielem, void *elem, void *udt), void *udt)
{
    struct m_avlnode **link = NULL;
    struct m_avlnode *parent = NULL;
    struct m_avlnode *node = NULL;
    if (!tree || !elem || !cbk) return M_EINVAL;

    link = &tree->root;
    while (*link) {
        int ret = 0;
        parent = *link;
        ret =  cbk(NODE2ELEM(parent,tree->offset), elem, udt);
        if (ret > 0)
            link = &parent->left;
        else if (ret < 0)
            link = &parent->right;
        else
            return M_EEXISTS;
    }

    node = ELEM2NODE(elem,tree->offset);
    node->left = node->right = NULL;
    node->parent = parent;
    node->height = 0;
    *link = node;

    /* rebalance avltree */
    insert_rebalance(node, &tree->root);
    
    tree->count++;

    return 0;
}

int m_avltree_remove(struct m_avltree *tree, void *elem)
{
    if (!tree || !elem) return M_EINVAL;

    struct m_avlnode *node = ELEM2NODE(elem,tree->offset);
    node_remove(node, &tree->root);
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->height = 0;

    return 0;
}

void *m_avltree_find(struct m_avltree *tree, void *key,
                int (*cbk)(void *ielem, void *key, void *udt), void *udt)
{
    if (!tree || !cbk) return NULL;

    struct m_avlnode *node = tree->root;
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

void *m_avltree_prev(struct m_avltree *tree, void *elem)
{
    if (!tree || !elem) return NULL;

    struct m_avlnode *node = ELEM2NODE(elem,tree->offset);
        
	if (node->left) {
		node = node->left;
		while (node->right) 
			node = node->right;
	} else {
        while (1) {
			struct m_avlnode *child = node;
			node = node->parent;
			if (node == NULL)
			    return NULL;
			if (node->right == child)
			    break;
		}
	}

    return NODE2ELEM(node,tree->offset);
}

void *m_avltree_next(struct m_avltree *tree, void *elem)
{
    if (!tree || !elem) return NULL;

    struct m_avlnode *node = ELEM2NODE(elem,tree->offset);

    if (node->right) {
		node = node->right;
		while (node->left) 
			node = node->left;
	} else {
        while (1) {
			struct m_avlnode *child = node;
			node = node->parent;
			if (node == NULL)
			    return NULL;
			if (node->left == child)
			    break;
		}
	}

    return NODE2ELEM(node,tree->offset);
}

void *m_avltree_first(struct m_avltree *tree)
{
    if (!tree) return NULL;
    if (!tree->root) return NULL;
    
    struct m_avlnode *node = tree->root;
    while (node->left)
        node = node->left;

    return NODE2ELEM(node,tree->offset);
}

void *m_avltree_last(struct m_avltree *tree)
{
    if (!tree) return NULL;
    if (!tree->root) return NULL;

    struct m_avlnode *node = tree->root;
    while (node->right)
        node = node->right;

    return NODE2ELEM(node,tree->offset);
}

void *m_avltree_root(struct m_avltree *tree)
{
    if (!tree) return NULL;
    if (!tree->root) return NULL;

    return NODE2ELEM(tree->root,tree->offset);
}

static void avltree_inorder(size_t offset, struct m_avlnode *node,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!node) return;
    avltree_inorder(offset, node->left, cbk, udt);
    cbk(NODE2ELEM(node,offset), udt);
    avltree_inorder(offset, node->right, cbk, udt);
}

void m_avltree_inorder(struct m_avltree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!tree || !cbk) return;

    return avltree_inorder(tree->offset, tree->root, cbk, udt);
}

static void avltree_preorder(size_t offset, struct m_avlnode *node,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!node) return;
    cbk(NODE2ELEM(node,offset), udt);
    avltree_preorder(offset, node->left, cbk, udt);
    avltree_preorder(offset, node->right, cbk, udt);
}

void m_avltree_preorder(struct m_avltree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!tree || !cbk) return;

    return avltree_preorder(tree->offset, tree->root, cbk, udt);
}

static void avltree_postorder(size_t offset, struct m_avlnode *node,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!node) return;
    avltree_postorder(offset, node->left, cbk, udt);
    avltree_postorder(offset, node->right, cbk, udt);
    cbk(NODE2ELEM(node,offset), udt);
}

void m_avltree_postorder(struct m_avltree *tree,
                    void (*cbk)(void *elem, void *udt), void *udt)
{
    if (!tree || !cbk) return;

    return avltree_postorder(tree->offset, tree->root, cbk, udt);
}

static int avltree_judge(struct m_avlnode *node, int *height)
{
    if (!node) {
        *height = 0;
        return 0;
    }
    
    int lheight;  
    int lresult = avltree_judge(node->left, &lheight);  
    int rheight;  
    int rresult = avltree_judge(node->right, &rheight);

    *height = MAX(lheight, rheight) + 1;
    if(!lresult && !rresult && ABS(lheight - rheight) < 2)
        return 0;
    else
        return -1;
}

int m_avltree_judge(struct m_avltree *tree)
{
    if (!tree) return M_EINVAL;

    int height = 0;
    return avltree_judge(tree->root, &height);  
}
