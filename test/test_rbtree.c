
#include <stdio.h>

#include "rbtree.h"

struct element {
    int key;
    struct m_rbnode rbnode;
};

void cbk_free(void *elem, void *udt)
{
    struct element *em = (struct element *)elem;
    free(em);
}

int cbk_insert(void *ielem, void *elem, void *udt)
{
    struct element *ie = (struct element *)ielem;
    struct element *e = (struct element *)elem;
    if (ie->key > e->key)
        return 1;
    else if (ie->key < e->key)
        return -1;
    else
        return 0;
}

void cbk_inoder(void *elem, void *udt)
{
    /*
    struct m_avlnode *node = (struct m_avlnode *)((size_t)(elem)+(size_t)&((struct element *)0)->avlnode);
    */
    struct element *e = (struct element *)elem;
    /*
    printf("%d:%d ", e->key, node->height);
    */
    printf("%d ", e->key);
}

int cbk_find(void *ielem, void *key, void *udt)
{
    int k = (int)(long)key;
    struct element *elm = (struct element *)ielem;
    if (elm->key > k)
        return 1;
    else if (elm->key < k)
        return -1;
    else
        return 0;
}

int main()
{
    int i = 0;
    int ret = 0;
    struct m_rbtree tree = {0};
    struct element *elem = NULL;
    struct element *temp = NULL;

    ret = m_rbtree_init(&tree, M_RBTREE_OFFSET(struct element,rbnode));
    if (ret) {
        printf("m_rbtree_init() failed: %d\n", ret);
        return -1;
    }

    srand(5);
    for (i = 7; i > 0; i--) {
        int n = rand() % 100;
        struct element *elm = (struct element *)malloc(sizeof(struct element));
        elm->key = n;
        /* printf("m_avltree_insert %d\n", n); */
        ret = m_rbtree_insert(&tree, elm, cbk_insert, NULL);
        if (ret) {
            if (ret == M_EEXISTS)
                printf("m_rbtree_insert failed:%d, %d already in tree\n", ret, n);
            else
                printf("m_rbtree_insert failed:%d\n", ret);
        }
    }

    /* test judge */
    if (m_rbtree_judge(&tree))
        printf("is not an rbtree\n");
    else
        printf("is an rbtree\n");

    /* test inorder */
    printf("inorder:");
    m_rbtree_inorder(&tree, cbk_inoder, NULL);
    printf("\n");
    /* test preorder */
    printf("preorder:");
    m_rbtree_preorder(&tree, cbk_inoder, NULL);
    printf("\n");
    /* test postorder */
    printf("postorder:");
    m_rbtree_postorder(&tree, cbk_inoder, NULL);
    printf("\n");

    /* test remove */
    i = 76;
    elem = m_rbtree_find(&tree, (void *)(long)i, cbk_find, NULL);
    if (!elem) {
        printf("can not remove:%d\n", i);
    } else {
        m_rbtree_remove(&tree, elem);
    }
    printf("remove:");
    m_rbtree_inorder(&tree, cbk_inoder, NULL);
    printf("\n");
    /* test judge */
    if (m_rbtree_judge(&tree))
        printf("is not an rbtree\n");
    else
        printf("is an rbtree\n");

    /* test get */
    elem = m_rbtree_root(&tree);
    printf("root: %d\n", elem->key);
    elem = m_rbtree_first(&tree);
    printf("first: %d\n", elem->key);
    elem = m_rbtree_last(&tree);
    printf("last: %d\n", elem->key);
    i = 10;
    elem = m_rbtree_find(&tree, (void *)(long)i, cbk_find, NULL);
    if (!elem)
        printf("can not find:%d\n", i);
    else
        printf("find: %d\n", elem->key);
    if (elem) {
        temp = m_rbtree_prev(&tree, elem);
        if (!temp)
            printf("elem:%d is first, have not prev\n", elem->key);
        else
            printf("elem:%d prev is:%d\n", elem->key, temp->key);
        temp = m_rbtree_next(&tree, elem);
        if (!temp)
            printf("elem:%d is last, have not next\n", elem->key);
        else
            printf("elem:%d next is:%d\n", elem->key, temp->key);
    }
    
    /* test free */
    ret = m_rbtree_free(&tree, cbk_free, NULL);
    if (ret) {
        printf("m_rbtree_free() failed: %d\n", ret);
        return -1;
    }

    return 0;
}
