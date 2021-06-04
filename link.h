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
} LINK;

LINK* create_link();
int move_front(LINK *Link, linknode *x);
linknode* add_front(LINK *Link, char *key, unsigned short type, Node *rt);
int link_delnode(LINK* Link, linknode* node);
int link_popback(LINK* Link);

extern LINK *lru;

#endif