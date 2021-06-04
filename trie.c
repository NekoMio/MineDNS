#include "trie.h"

#include "log.h"
#include "malloc.h"

short trans(char x) {
  if (x == '.') return 36;
  if (x == '-') return 37;
  if (x <= '9' && x >= '0') return x - '0';
  if (x >= 'a' && x <= 'z') return x - 'a' + 10;
  return -1;
}

TNode *NewTrieNode() {
  TNode *rt = malloc(sizeof(TNode));
  memset(rt->ch, 0, sizeof(rt->ch));
  rt->size = 0;
  rt->flag = 0;
  rt->data = NULL;
  return rt;
}

TrieRoot *create_trie() {
  TrieRoot *rt = malloc(sizeof(TrieRoot));
  for (int i = 0; i < 34; i++) rt->node[i] = NULL;
  // rt->node = NewTrieNode();
  return rt;
}

void freeTrienode(TNode *rt) {
  for (int i = 0; i < 38; i++) {
    if (rt->ch[i] != NULL) {
      freeTrienode(rt->ch[i]);
    }
  }
  if (rt->data != NULL) free(rt->data);
  free(rt);
}

void free_trie(TrieRoot *root) {
  for (int i = 0; i <= 33; i++) {
    if (root->node[i])
      freeTrienode(root->node[i]);
  }
}

TNode *insert_trie(TrieRoot *root, char *key, unsigned short type, char *data,
                   int len) {
  int keylen = strlen(key);
  if (type > 33) return NULL;
  if (root->node[type] == NULL) {
    root->node[type] = NewTrieNode();
  }
  TNode *rt = root->node[type];
  rt->size++;
  for (int i = 0; i < keylen; i++) {
    if (!rt->ch[trans(key[i])]) {
      rt->ch[trans(key[i])] = NewTrieNode();
    }
    rt = rt->ch[trans(key[i])];
    rt->size++;
  }
  rt->flag = 1;
  rt->data = malloc(len);
  rt->len = len;
  memcpy(rt->data, data, len);
}

char *search_trie(TrieRoot *root, unsigned short type, char *key, int *len) {
  int keylen = strlen(key);
  if (type > 33 || root->node[type] == NULL) {
    *len = -1;
    return NULL;
  }
  TNode *rt = root->node[type];
  for (int i = 0; i < keylen; i++) {
    if (!rt->ch[trans(key[i])]) {
      *len = -1;
      return NULL;
    }
    rt = rt->ch[trans(key[i])];
  }
  if (rt->flag) {
    *len = rt->len;
    if (*len == 0) return NULL;
    return rt->data;
  } else {
    *len = -1;
    return NULL;
  }
}

void delete_trie(TrieRoot *root, char *key, unsigned short type) {
  int keylen = strlen(key);
  if (root->node[type] == NULL) return;
  TNode *rt = root->node[type];
  TNode *delRT = NULL;
  for (int i = 0; i < keylen; i++) {
    if (!rt->ch[trans(key[i])]) {
      return;
    }
    rt->size--;
    if (rt->size == 0) {
      delRT = rt;
    }
  }
  rt->flag = 0;
  free(rt->data);
  if (delRT != NULL) {
    freeTrienode(delRT);
  }
}
