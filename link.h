#ifndef linkH
#define linkH

#include "tree.h"

typedef struct _linknode {
	struct _linknode *pre, *nxt;
	char* key;
	unsigned short type;
    Node *rt;
} linknode;

typedef struct _link {
	linknode *head, *tail;
	int length;
} link;

link* create_link();
int move_front(link *Link, linknode *x);
linknode* add_front(link *Link, char *key, unsigned short type, Node *rt);
int link_delnode(link* Link, linknode* node);
int link_popback(link* Link);

extern link *lru;

#endif