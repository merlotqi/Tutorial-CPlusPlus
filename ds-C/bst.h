#pragma once

typedef struct bst_node_s {
    int data;
    struct bst_node_s *left, *right;
} bst_node_t;

typedef struct bst_s {
    bst_node_t *root;
} bst_t;

bst_t *bst_new();
void bst_destroy(bst_t *bst);

void bst_insert(bst_t *bst, int data);
void bst_delete(bst_t *bst, int data);
bst_node_t *bst_search(bst_t *bst, int data);

void bst_print(bst_node_t *bst);