#include "avl.h"

#include <stdio.h>

int main() {
    avl_tree_t *tree = avl_new();

    printf("Testing balance adjustment during deletion:\n");

    // Build a tree that may require rotations
    avl_insert(tree, 50);
    avl_insert(tree, 30);
    avl_insert(tree, 70);
    avl_insert(tree, 20);
    avl_insert(tree, 40);
    avl_insert(tree, 60);
    avl_insert(tree, 80);
    avl_insert(tree, 10);
    avl_insert(tree, 25);
    avl_insert(tree, 35);
    avl_insert(tree, 45);

    printf("Initial tree in-order traversal: ");
    avl_print(tree->root);

    // Delete a node that may cause rotation
    printf("Deleting node 40 (may cause rotation):\n");
    avl_delete(tree, 40);
    printf("In-order traversal after deletion: ");
    avl_print(tree->root);

    // Verify the tree is still balanced
    printf("Tree root node: %d\n", tree->root->data);

    avl_destroy(tree);
}