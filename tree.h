#ifndef treeH
#define treeH

#include "time.h"
#include <string.h>


typedef time_t Type;

typedef struct NodeData {
  int s, key;
  Type x;
  char *value;
  unsigned short type;
  struct NodeData *ch[2], *fa;
#define size(_) ((_) ? (_)->s : 0)
}Node;

typedef struct rbroot {
    Node *node;
}RBRoot;

RBRoot *create_tree();

void free_tree(RBRoot *root);

Node *insert_tree(RBRoot *root, Type key, char* value, unsigned short type);

void delete_tree(RBRoot *root, Type key);

const char *search_tree(RBRoot *root, Type key);

void delete_treenode(RBRoot *root, Node *rt);

typedef RBRoot Map;

extern Map *timeout;

#endif