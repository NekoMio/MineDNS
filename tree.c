#include "tree.h"

#include <stdlib.h>

RBRoot *create_tree() {
  RBRoot *root = malloc(sizeof(RBRoot));
  root->node = NULL;

  return root;
}

Node *search(RBTree x, Type key) {
  if (x == NULL || x->key == key) {
    return x;
  }
  if (key < x->key) {
    return search(x->left, key);
  } else {
    return search(x->right, key);
  }
}

const char *search_tree(RBRoot *root, Type key) {
  if (root) {
    RBTree ret = search(root->node, key);
    if (ret == NULL)
      return NULL;
    else
      return ret->value;
  }
  return NULL;
}

void left_rotate(RBRoot *root, Node *x) {
  Node *y = x->right;
  x->right = y->left;
  if (y->left != NULL) {
    y->left->parent = x;
  }
  y->parent = x->parent;

  if (x->parent == NULL) {
    root->node = y;
  } else {
    if (x->parent->left == x) {
      x->parent->left = y;
    } else {
      x->parent->right = x;
    }
  }

  y->left = x;
  x->parent = y;
}

void right_rotate(RBRoot *root, Node *y) {
  Node *x = y->left;
  y->left = x->right;
  if (x->right != NULL) {
    x->right->parent = y;
  }
  x->parent = y->parent;

  if (y->parent == NULL) {
    root->node = x;
  } else {
    if (y == y->parent->right) {
      y->parent->right = x;
    } else {
      y->parent->left = x;
    }
  }
  x->right = y;
  y->parent = x;
}

void insert_fixup(RBRoot *root, Node *node) {
  Node *parent, *gparent;
  while ((parent = node->parent) && parent->color == RED) {
    gparent = parent->parent;
    if (parent == gparent->left) {
      {
        Node *uncle = gparent->right;
        if (uncle && uncle->color == RED) {
          uncle->color = BLACK;
          parent->color = BLACK;
          gparent->color = RED;
          node = gparent;
          continue;
        }
      }
      if (parent->right == node) {
        Node *tmp;
        left_rotate(root, parent);
        tmp = parent;
        parent = node;
        node = tmp;
      } else {
        parent->color = BLACK;
        gparent->color = RED;
        right_rotate(root, gparent);
      }
    } else {
      {
        Node *uncle = gparent->left;
        if (uncle && uncle->color == RED) {
          uncle->color = BLACK;
          parent->color = BLACK;
          gparent->color = RED;
          node = parent;
          continue;
        }
      }
      if (parent->left == node) {
        Node *tmp;
        right_rotate(root, parent);
        tmp = parent;
        parent = node;
        node = tmp;
      } else {
        parent->color = BLACK;
        gparent->color = RED;
        left_rotate(root, gparent);
      }
    }
  }

  root->node->color = BLACK;
}

void insertnode(RBRoot *root, Node *node) {
  Node *y = NULL;
  Node *x = root->node;
  while (x != NULL) {
    y = x;
    if (node->key < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }
  node->parent = y;
  if (y != NULL) {
    if (node->key < y->key) {
      y->left = node;
    } else {
      y->right = node;
    }
  } else {
    root->node = node;
  }
  root->node = RED;

  insert_fixup(root, node);
}

Node *NewNode(Type key, char *value, Node *parent, Node *left, Node *right) {
  Node *p;
  if ((p = malloc(sizeof(Node))) == NULL) {
    return NULL;
  }
  p->key = key;
  p->value = strdup(value);
  p->left = left;
  p->right = right;
  p->parent = parent;
  p->color = BLACK;

  return p;
}

Node *insert_tree(RBRoot *root, Type key, char* value) {
  Node *node;
  if ((node = search(root->node, key)) != NULL) {
    return node;
  }
  if ((node = NewNode(key, value, NULL, NULL, NULL)) == NULL) return NULL;

  insertnode(root, node);

  return node;
}

void delete_fixup(RBRoot *root, Node *node, Node *parent) {
  Node *other;
  while ((!node || node->color == BLACK) && (node != root->node)) {
    if (parent->left == node) {
      other = parent->right;
      if (other->color == RED) {
        other->color = BLACK;
        parent->color = RED;
        left_rotate(root, parent);
        other = parent->right;
      }
      if ((!other->left || other->left->color == BLACK) &&
          (!other->right || other->right->color == BLACK)) {
        other->color = RED;
        node = parent;
        parent = node->parent;
      } else {
        if (!other->right || other->right->color == BLACK) {
          other->left->color = BLACK;
          other->color = RED;
          right_rotate(root, other);
          other = parent->right;
        } else {
          other->color = parent->color;
          parent->color = BLACK;
          other->right->color = BLACK;
          left_rotate(root, parent);
          node = root->node;
          // break;
        }
      }
    } else {
      other = parent->left;
      if (other->color == RED) {
        other->color = BLACK;
        parent->color = RED;
      }
      if ((!other->left || other->left->color == BLACK) &&
          (!other->right || other->right->color == RED)) {
        other->color = RED;
        node = parent;
        parent = node->parent;
      }
      else {
        if (!other->left || other->left->color == BLACK) {
          other->right->color = BLACK;
          other->color = RED;
          left_rotate(root, other);
          other = parent->left;
        } else {
          other->color = parent->color;
          parent->color = BLACK;
          other->left->color = BLACK;
          right_rotate(root, parent);
          node = root->node;
          // break;
        }
      }
    }
  }
  if (node) {
    node->color = BLACK;
  }
}

void deletenode(RBRoot *root, Node *node) {
  Node *child, *parent;
  int color;
  if ((node->left != NULL) && (node->right != NULL)) {
    Node *replace = node;
    replace = replace->right;
    while (replace->left != NULL) replace = replace->left;
    if (node->parent != NULL) {
      if (node->parent->left == node) {
        node->parent->left = replace;
      } else {
        node->parent->right = replace;
      }
    } else {
      root->node = replace;
    }

    child = replace->right;
    parent = replace->parent;
    color = replace->color;

    if (parent == node) {
      parent = replace;
    } else {
      if (child) child->parent = parent;
      parent->left = child;

      replace->right = node->right;
      node->right->parent = replace;
    }
    replace->parent = node->parent;
    replace->color = node->color;
    replace->left = node->left;
    node->left->parent = replace;

    if (color == BLACK) {
      delete_fixup(root, child, parent);
    }
    free(node->value);
    free(node);
    return;
  }
  if (node->left != NULL) {
    child = node->left;
  } else {
    child = node->right;
  }
  parent = node->parent;
  color = node->color;

  if (child) {
    child->parent = parent;
  }

  if (parent) {
    if (parent->left == node) {
      parent->left = child;
    } else {
      parent->right = child;
    }
  } else {
    root->node = child;
  }
  if (color == BLACK) {
    delete_fixup(root, child, parent);
  }
  free(node->value);
  free(node);
}

void delete_tree(RBRoot *root, Type key) {
  Node *node;
  if ((node = search(root->node, key)) != NULL) {
    deletenode(root, node);
  }
}

void freenode(Node *root) {
  if (root->left != NULL) {
    freenode(root->left);
  }
  if (root->right != NULL) {
    freenode(root->right);
  }
  free(root);
}
void free_tree(RBRoot *root) {
  if (root != NULL && root->node != NULL) {
    freenode(root->node);
  }

  free(root);
}