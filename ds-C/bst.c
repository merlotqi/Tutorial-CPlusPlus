#include "bst.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bst_node_t *_bst_node_new(int data) {
    bst_node_t *node = malloc(sizeof(bst_node_t));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static bst_node_t *_bst_insert(bst_node_t *node, int value) {
    if (node == NULL) {
        return _bst_node_new(value);
    }

    if (value < node->data) {
        node->left = _bst_insert(node->left, value);
    } else {
        node->right = _bst_insert(node->right, value);
    }

    return node;
}

static bst_node_t *_bst_search(bst_node_t *node, int value) {
    if (node == NULL || node->data == value) {
        return node;
    }

    if (value < node->data) {
        return _bst_search(node->left, value);
    } else {
        return _bst_search(node->right, value);
    }

    return NULL;
}

static void _bst_destroy(bst_node_t *node) {
    if (node == NULL) {
        return;
    }

    _bst_destroy(node->left);
    _bst_destroy(node->right);
    free(node);
}

static bst_node_t *_bst_find_right_min(bst_node_t *node) {
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

static bst_node_t *_bst_delete(bst_node_t *node, int value) {
    if (node == NULL) {
        return NULL;
    }

    if (value < node->data) {
        node->left = _bst_delete(node->left, value);
    } else if (value > node->data) {
        node->right = _bst_delete(node->right, value);
    } else {
        if (node->left == NULL) {
            bst_node_t *temp = node->right;
            free(node);
            node = temp;
        } else if (node->right == NULL) {
            bst_node_t *temp = node->left;
            free(node);
            node = temp;
        }

        bst_node_t *rm = _bst_find_right_min(node->right);
        node->data = rm->data;
        node->right = _bst_delete(node->right, rm->data);
    }
    return node;
}

bst_t *bst_new() {
    bst_t *bst = malloc(sizeof(bst_t));
    if (bst == NULL) {
        return NULL;
    }
    bst->root = NULL;
    return bst;
}

void bst_destroy(bst_t *bst) {
    _bst_destroy(bst->root);
    free(bst);
}

void bst_insert(bst_t *bst, int data) {
    bst->root = _bst_insert(bst->root, data);
}

void bst_delete(bst_t *bst, int data) {}

bst_node_t *bst_search(bst_t *bst, int data) {
    return _bst_search(bst->root, data);
}

void bst_print(bst_node_t *node) {
    if (node != NULL) {
        bst_print(node->left);
        printf("%d\n", node->data);
        bst_print(node->right);
    }
}
