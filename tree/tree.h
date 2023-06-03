#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define max(a,b) (((a) > (b)) ? (a) : (b))

/* Data Structure Prototypes */

typedef struct tree tree;
typedef enum tree_direction tree_direction;
typedef struct route_data route_data;

/* Data structure definitions */

struct tree {
    void *data;
    tree *left;
    tree *right;
    int height;
};

enum tree_direction {
    TREE_LEFT = -1,
    TREE_RIGHT = 1,
    TREE_INVALID = 0
};

struct route_data {
    char *route;
    char *file;
};


/* Function Prototypes */

// Create a new node
tree*   tree_new_node       (void *data);

// Create a tree with a root node
tree*   tree_create         (void *root_data);

// Destroy a tree
int*    tree_destroy        (tree *t);

// Get the height of a tree
int     tree_height         (tree *t);

// Get the leftmost tree node
tree*   tree_left_most      (tree *t);

// Get the rightmost tree node
tree*   tree_right_most     (tree *t);

// Balance a tree
tree*   tree_balance        (tree *t);

// Insert a node into a tree
tree*   tree_insert         (tree *t, void *data, tree_direction (*cmp_fn)(void*, void*));

// Delete a node from a tree
tree*   tree_delete         (tree *t, void *data, tree_direction (*cmp_fn)(void*, void*));

// Get a node from a tree
tree*   tree_get_node       (tree *t, void *data, tree_direction (*cmp_fn)(void*, void*));

// Get the data from a tree
void*   tree_get            (tree *t, void *data, tree_direction (*cmp_fn)(void*, void*));

// Rotate a tree
tree*   tree_rotate         (tree *root, tree_direction direction);

// Get the balance factor of a tree
int     tree_balance_factor (tree *t);

// Print tree in using parenthesis notation
void    tree_print          (tree *t, void (*print_fn)(void*));

#endif // TREE_H