#pragma once

typedef struct avl_node_s {
    int data;
    struct avl_node_s *left, *right;
    int height;
} avl_node_t;

typedef struct avl_tree_s {
    avl_node_t *root;
} avl_tree_t;

avl_tree_t *avl_new();
void avl_destroy(avl_tree_t *tree);

void avl_insert(avl_tree_t *tree, int data);
void avl_delete(avl_tree_t *tree, int data);
avl_node_t *avl_search(avl_tree_t *tree, int data);

void avl_print(avl_node_t *node);