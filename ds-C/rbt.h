#pragma once

#define RED 0
#define BLACK 1

typedef struct rbt_node_s {
    int data;
    int color;
    struct rbt_node_s *left, *right, *parent;
} rbt_node_t;

typedef struct rbt_s {
    rbt_node_t *root;
} rbt_t;

rbt_t *rbt_new();
void rbt_destroy(rbt_t *rbt);

void rbt_insert(rbt_t *rbt, int data);
void rbt_delete(rbt_t *rbt, int data);
rbt_node_t *rbt_search(rbt_t *rbt, int data);

void rbt_print(rbt_node_t *node);
