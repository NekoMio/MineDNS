#ifndef treeH
#define treeH

#include <string.h>

enum { RED, BLACK };

typedef long long Type;

typedef struct RBTreeNode {
    unsigned char color;
    Type key;
    char *value;
    struct RBTreeNode *left;
    struct RBTreeNode *right;
    struct RBTreeNode *parent;
}Node, *RBTree;

typedef struct rbroot {
    Node *node;
}RBRoot;

RBRoot *create_tree();

void free_tree(RBRoot *root);

Node *insert_tree(RBRoot *root, Type key, char* value);

void delete_tree(RBRoot *root, Type key);

const char *search_tree(RBRoot *root, Type key);

typedef RBRoot Map;

#endif