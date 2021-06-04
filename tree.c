#include "tree.h"
#include "trie.h"
#include "log.h"

#include <stdlib.h>

Node *NewNode(int x, unsigned short type, char *value) {
  Node *rt = malloc(sizeof(Node));
  rt->s = 1, rt->key = rand();
  rt->x = x;
  rt->type = type;
  rt->value = strdup(value);
  rt->ch[0] = rt->ch[1] = NULL;
  rt->fa = NULL;
}
void Pushup(Node *rt) {
  rt->s = size(rt->ch[0]) + size(rt->ch[1]) + 1;
  if (rt->ch[0]) rt->ch[0]->fa = rt;
  if (rt->ch[1]) rt->ch[1]->fa = rt;
}
Node *Merge(Node *A, Node *B) {
  if (!A) return B;
  if (!B) return A;
  if (A->key < B->key) {
    A->ch[1] = Merge(A->ch[1], B);
    // if (A->ch[1]) A->ch[1]->fa = A;
    Pushup(A);
    return A;
  } else {
    B->ch[0] = Merge(A, B->ch[0]);
    Pushup(B);
    return B;
  }
}
typedef struct Node2 {
  Node *first, *second;
} DNode;
DNode Split(Node *rt, int k) {
  if (!rt) return (DNode){NULL, NULL};
  DNode o;
  if (size(rt->ch[0]) >= k) {
    o = Split(rt->ch[0], k);
    rt->ch[0] = o.second;
    Pushup(rt);
    o.second = rt;
  } else {
    o = Split(rt->ch[1], k - size(rt->ch[0]) - 1);
    rt->ch[1] = o.first;
    Pushup(rt);
    o.first = rt;
  }
  return o;
}
Node *kth(RBRoot *rt, int k) {
  DNode x = Split(rt->node, k - 1);
  DNode y = Split(x.second, 1);
  Node *ans = y.first;
  rt->node = Merge(Merge(x.first, ans), y.second);
  return ans;
}
int Rank(Node *rt, time_t x) {
  if (!rt) return 0;
  return x < rt->x ? Rank(rt->ch[0], x)
                    : Rank(rt->ch[1], x) + size(rt->ch[0]) + 1;
}
void Insert(RBRoot *rt, Node *x) {
  int k = Rank(rt->node, x->x);
  DNode y = Split(rt->node, k);
  rt->node = Merge(Merge(y.first, x), y.second);
}

RBRoot *create_tree() {
  RBRoot *rt = malloc(sizeof(RBRoot));
  rt->node = NULL;
  return rt;
}

Node *insert_tree(RBRoot *root, Type key, char *value, unsigned short type) {
  Node *x = NewNode(key, type, value);
  Insert(root, x);
  return x;
}

Node *findtoswap(Node *rt) {
  if (rt->ch[0] == NULL) return rt;
  return findtoswap(rt->ch[0]);
}

void delete_treenode(RBRoot *root, Node *rt) {
  if (rt->ch[0] == NULL || rt->ch[1] == NULL) {
    if (rt->fa != NULL) {
        ((rt->fa->ch[0] == rt) ? (rt->fa->ch[0] = (rt->ch[0] ? rt->ch[0] : rt->ch[1])) : (rt->fa->ch[1] = (rt->ch[0] ? rt->ch[0] : rt->ch[1])));
        ((rt->ch[0]) ? (rt->ch[0]->fa = rt->fa) : (rt->ch[1]->fa = rt->fa));
    } else {
      if (rt->ch[0]) {
        root->node = rt->ch[0];
        rt->ch[0]->fa = NULL;
      } else {
        root->node = rt->ch[1];
        rt->ch[1]->fa = NULL;
      }
    }
    return;
  } else {
    Node *twp = findtoswap(rt->ch[1]);
    Node tmp;
    tmp = *twp;
    *twp = *rt;
    *rt = tmp;
    twp->key = rt->key;
    rt->key = tmp.key;
    delete_treenode(root, rt);
  }
}

void DFS_Delete(Node *rt) {
  LOG(DEBUGMSG, "%s type:%d TTL timeout delete from cache\n", rt->value, rt->type);
  delete_trie(cacheData, rt->value, rt->type);
  if (rt->ch[0] != NULL) DFS_Delete(rt->ch[0]);
  if (rt->ch[1] != NULL) DFS_Delete(rt->ch[1]);
}

void delete_tree(RBRoot *root, Type key) {
  int k = Rank(root->node, key);
  if (k != 0) {
    DNode y = Split(root->node, k);
    DFS_Delete(y.first);
  }
}

// Node *search(Node *rt, Type key) {
//   if (rt == NULL || rt->key == key) {
//     // return x;
//   }
// }

// const char *search_tree(RBRoot *root, Type key) {

// }