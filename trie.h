#ifndef trieH
#define trieH

#include <string.h>


short trans(char x);

typedef struct TrieNode {
    struct TrieNode *ch[38];
    int size, len;
    int flag;
    char *data;
}TNode, *Trie;

typedef struct trieroot {
    TNode *node;
}TrieRoot;

TrieRoot *create_trie();

void free_trie(TrieRoot *root);

TNode *insert_trie(TrieRoot *root, char *key, char *data, int len);

void delete_trie(TrieRoot *root, char *key);

char *search_trie(TrieRoot *root, char *key, int *len);

TrieRoot *cacheData;

#endif