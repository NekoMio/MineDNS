#include "link.h"
#include "trie.h"
#include "stdlib.h"
#include "log.h"

LINK *create_link() {
    LINK *ls = malloc(sizeof(LINK));
    ls->head = NULL;
    ls->tail = NULL;
    ls->length = 0;
    return ls;
}

int move_front(LINK *Link, linknode *x) {
    if (Link->head == x) return 0;
    if (x->pre) {
        x->pre->nxt = x->nxt;
    }
    if (x->nxt) {
        x->nxt->pre = x->pre;
    }
    if (Link->head) {
        Link->head->pre = x;
    }
    x->nxt = Link->head;
    x->pre = NULL;
    Link->head = x;
}

linknode *add_front(LINK *Link, char *key, unsigned short type, Node *rt) {
    linknode *x = malloc(sizeof(linknode));
    x->key = strdup(key);
    x->type = type;
    x->rt = rt;
    if (Link->head) {
        Link->head->pre = x;
    }
    x->nxt = Link->head;
    x->pre = NULL;
    Link->head = x;
    Link->length++;
    return x;
}
int link_delnode(LINK* Link, linknode* node) {
    if (node == NULL) return 0;
    if (node->pre) {
        node->pre->nxt = node->nxt;
    }
    if (node->nxt) {
        node->nxt->pre = node->pre;
    }
    if (Link->head == node) {
        Link->head = node->nxt;
    }
    if (Link->tail == node) {
        Link->tail = node->pre;
    }
    node->pre = NULL;
    node->nxt = NULL;
    free(node->key);
    free(node);
    Link->length--;
    return 1;
}
int link_popback(LINK* Link) {
    if (Link->tail == NULL) return 0;
    LOG(DEBUGMSG, "lru: Delete Cache %s: %d\n", Link->tail->key, Link->tail->type);
    delete_trie(cacheData, Link->tail->key, Link->tail->type);
    delete_treenode(timeout, Link->tail->rt);
    free(Link->tail->key);
    if (Link->tail->pre) Link->tail->pre->nxt = NULL;
    linknode *tmp = Link->tail;
    Link->tail = Link->tail->pre;
    free(tmp);
    Link->length--;
}
