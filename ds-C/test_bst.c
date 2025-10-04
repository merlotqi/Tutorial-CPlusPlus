#include "bst.h"
#include <stdio.h>

void main() {
    bst_t *bst = bst_new();

    bst_insert(bst, 10);
    bst_insert(bst, 5);
    bst_insert(bst, 15);
    bst_insert(bst, 3);
    bst_insert(bst, 7);
    bst_insert(bst, 12);
    bst_insert(bst, 18);

    bst_print(bst->root);

    bst_node_t *node = bst_search(bst, 7);
    if (node) {
        printf("Found node with value %d\n", node->data);
    } else {
        printf("Node with value %d not found\n", 7);
    }

    bst_destroy(bst);
}