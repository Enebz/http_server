#include "tree.h"


/* Function Definitions */

tree *tree_new_node(void *data)
{
    tree *t = (tree*)malloc(sizeof(tree));
    t->data = data;
    t->left = NULL;
    t->right = NULL;
    t->height = 1;
    return t;
}

tree *tree_create(void *root_data)
{
    if (root_data == NULL)
    {
        return NULL;
    }

    return tree_new_node(root_data);
}

int *tree_destroy(tree *t)
{
    if (t == NULL)
    {
        return 0;
    }

    tree_destroy(t->left);
    tree_destroy(t->right);
    free(t);
    return 0;
}

int tree_height(tree *t)
{
    if (t == NULL)
    {
        return 0;
    }

    return t->height;
}

tree* tree_left_most(tree *t)
{
    tree* left_most = t;

    while (left_most->left != NULL)
    {
        left_most = left_most->left;
    }

    return left_most;
}

tree* tree_right_most(tree *t)
{
    tree* right_most = t;

    while (right_most->right != NULL)
    {
        right_most = right_most->right;
    }

    return right_most;
}

tree* tree_balance(tree *t)
{
    // Get balance factor
    int balance = tree_balance_factor(t);

    // Left heavy
    if (balance > 1)
    {
        int balance_left = tree_balance_factor(t->left);

        // Check if we need a double rotation
        if (balance_left < 0)
        {
            t->left = tree_rotate(t->left, TREE_LEFT);
        }

        return tree_rotate(t, TREE_RIGHT);
    }
    // Right heavy
    else if (balance < -1)
    {
        int balance_right = tree_balance_factor(t->right);

        // Check if we need a double rotation
        if (balance_right > 0)
        {
            t->right = tree_rotate(t->right, TREE_RIGHT);
        }

        return tree_rotate(t, TREE_LEFT);
    }

    // Already balanced
    return t;
}

tree* tree_insert(tree *t, void *data, tree_direction (*cmp_fn)(void*, void*))
{
    /* Notes */
    // When returning, we basically set the parents pointer which was passed in recursively

    /* Stage 1: BST Insertion */
    if (t == NULL)
    {
        return tree_new_node(data);
    }

    int cmp_result = cmp_fn(data, t->data);

    if (cmp_result == TREE_LEFT)
    {
        t->left = tree_insert(t->left, data, cmp_fn);
    }

    else if (cmp_result == TREE_RIGHT)
    {
        t->right = tree_insert(t->right, data, cmp_fn);
    }

    else
    {
        return t;
    }

    /* Stage 2: AVL Height track */
    t->height = 1 + max(tree_height(t->left), tree_height(t->right));
    
    /* Stage 3: AVL Balancing */
    return tree_balance(t);
}

tree* tree_delete(tree *t, void *data, tree_direction (*cmp_fn)(void*, void*))
{
    /* Notes */
    // When returning, we basically set the parents pointer which was passed in recursively
    // So, here it's relevant to return NULL, as it indicates that a node has been deleted.
    
    /* Stage 1: BST Deletion */
    if (t == NULL)
    {
        // Did not find any node with the corresponding data according to cmp_fn
        return NULL;
    }

    int cmp_result = cmp_fn(data, t->data);

    if (cmp_result == TREE_LEFT)
    {
        t->left = tree_delete(t->left, data, cmp_fn);
    }

    else if (cmp_result == TREE_RIGHT)
    {
        t->right = tree_delete(t->right, data, cmp_fn);
    }

    else
    {
        // Found node to delete.
        // Case 1: No or one child
        if (t->left == NULL || t->right == NULL)
        {
            tree* tmp = t->left ? t->left : t->right;

            // No childs
            if (tmp == NULL)
            {
                // Set 
                tmp = t;
                t = NULL;
            }
            else {
                // Copy values from the temporary to the tree
                // This includes the pointers to the childs' left and right node, if any.
                *t = *tmp;
            }
            free(tmp);
        }
        // Case 2: Two children
        else {
            // leftmost node in right subtree
            tree* smallest_in_right = tree_left_most(t->right);

            t->data = smallest_in_right->data;

            t->right = tree_delete(t->right, smallest_in_right->data, cmp_fn);
        }
    }

    if (t == NULL)
    {
        // Did not find any node with the corresponding data according to cmp_fn
        return NULL;
    }

    /* Stage 2: AVL Height track */
    t->height = 1 + max(tree_height(t->left), tree_height(t->right));

    /* Stage 3: AVL Balancing */
    return tree_balance(t);
}

tree* tree_get_node(tree *t, void *data, tree_direction (*cmp_fn)(void*, void*))
{
    /* Stage 1: BST Lookup */
    if (t == NULL)
    {
        return NULL;
    }

    int cmp_result = cmp_fn(data, t->data);

    if (cmp_result == TREE_LEFT)
    {
        return tree_get_node(t->left, data, cmp_fn);
    }

    else if (cmp_result == TREE_RIGHT)
    {
        return tree_get_node(t->right, data, cmp_fn);
    }

    else
    {
        return t;
    }
}

void* tree_get(tree *t, void *data, tree_direction (*cmp_fn)(void*, void*))
{
    tree* node = tree_get_node(t, data, cmp_fn);


    if (node == NULL)
    {
        return NULL;
    }
    
    return node->data;
}

tree* tree_rotate(tree *root, tree_direction direction)
{
    tree *new_root;

    if (direction == TREE_RIGHT)
    {
        new_root = root->left;
        tree *t2 = new_root->right;

        // Rotate
        root->left = t2;
        new_root->right = root;

        // Update heights
        root->height = 1 + max(tree_height(root->left), tree_height(root->right));
        new_root->height = 1 + max(tree_height(new_root->left), tree_height(new_root->right));        
    }
    else if (direction == TREE_LEFT)
    {
        new_root = root->right;
        tree *t2 = new_root->left;

        // Rotate
        root->right = t2;
        new_root->left = root;

        // Update heights
        root->height = 1 + max(tree_height(root->left), tree_height(root->right));
        new_root->height = 1 + max(tree_height(new_root->left), tree_height(new_root->right));
    }
    else {
        return root;
    }
    return new_root;
}

int tree_balance_factor(tree *t)
{
    // If tree is NULL, it is balanced
    if (t == NULL)
    {
        return 0;
    }

    // left and right node may be NULL
    if (t->left == NULL && t->right == NULL)
    {
        return 0;
    }

    // If left is NULL, the equation is 0 - right->height
    if (t->left == NULL)
    {
        return -t->right->height;
    }

    // If right is NULL, the equation is left->height - 0
    if (t->right == NULL)
    {
        return t->left->height;
    }

    // If both nodes exist, the equation is left->height - right->height
    return t->left->height - t->right->height;
}

void tree_print(tree *t, void (*print_fn)(void*))
{
    if (t == NULL)
    {
        return;
    }
    print_fn(t->data);
    

    printf("(");
    if (t->left != NULL)
    {
        tree_print(t->left, print_fn);
    }
    printf(")");

    printf("(");
    if (t->right != NULL)
    {
        tree_print(t->right, print_fn);
    }
    printf(")");
    return;
}

// Iterate whole tree and apply function with each nodes data
// -1 delete node and return -1
// 0 = continue iteration
// 1 = stop iteration and return 1
int tree_iter(tree *t, int (*iter_fn)(void*), tree_direction (*cmp_fn)(void*, void*))
{
    if (t == NULL)
    {
        return 0;
    }

    int result = iter_fn(t->data);

    if (result == -1)
    {
        tree_delete(t, t->data, cmp_fn);
        return -1;
    }

    if (result == 1)
    {
        return 1;
    }

    if (tree_iter(t->left, iter_fn, cmp_fn) == 1)
    {
        return 1;
    }

    if (tree_iter(t->right, iter_fn, cmp_fn) == 1)
    {
        return 1;
    }

    return result;
}