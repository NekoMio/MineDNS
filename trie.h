#ifndef trieH
#define trieH

#include <string.h>
#include <time.h>
#include "link.h"
#include "tree.h"

short trans(char x);

extern int cachesize;

typedef struct TrieNode {
  struct TrieNode *ch[38];
  int size, len;
  int flag;
  char *data;
  linknode *lkn;
  time_t ttl;
} TNode, *Trie;

typedef struct trieroot {
  TNode *node[34];
} TrieRoot;

TrieRoot *create_trie();

void free_trie(TrieRoot *root);

TNode *insert_trie(TrieRoot *root, char *key, unsigned short type, char *data,
                   int len, time_t ttl, Node *nrt);

void delete_trie(TrieRoot *root, char *key, unsigned short type);

char *search_trie(TrieRoot *root, unsigned short type, char *key, int *len, time_t *ttl);

TrieRoot *cacheData;

#endif