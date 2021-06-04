#ifndef treeH
#define treeH

#include "time.h"
#include <string.h>

enum { RED, BLACK };

typedef time_t Type;

typedef struct RBTreeNode {
    unsigned char color;
    Type key;
    char *value;
    unsigned short type;
    struct RBTreeNode *left;
    struct RBTreeNode *right;
    struct RBTreeNode *parent;
}Node, *RBTree;

typedef struct rbroot {
    Node *node;
}RBRoot;

RBRoot *create_tree();

void free_tree(RBRoot *root);

Node *insert_tree(RBRoot *root, Type key, char* value, unsigned short type);

void delete_tree(RBRoot *root, Type key);

void deletenode(RBRoot *root, Node *node);

Node *search_min(RBRoot *root);

const char *search_tree(RBRoot *root, Type key);

typedef RBRoot Map;

extern Map *lru;
extern Map *timeout;

#endif