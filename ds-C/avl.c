#include "avl.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

static int _avl_node_height(avl_node_t *node) {
    return node ? node->height : 0;
}

static int _avl_balance_factor(avl_node_t *node) {
    if (node == NULL) return 0;
    return _avl_node_height(node->left) - _avl_node_height(node->right);
}

static void _avl_update_height(avl_node_t *node) {
    if (node == NULL) return;
    node->height = 1 + MAX(_avl_node_height(node->left), _avl_node_height(node->right));
}

static avl_node_t *_avl_node_new(int data) {
    avl_node_t *node = (avl_node_t *) malloc(sizeof(avl_node_t));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

static void _avl_destroy_node(avl_node_t *node) {
    if (node == NULL) return;
    _avl_destroy_node(node->left);
    _avl_destroy_node(node->right);
    free(node);
}

static avl_node_t *_avl_rotate_right(avl_node_t *node) {
    assert(node != NULL && node->left != NULL);

    avl_node_t *node_left = node->left;
    avl_node_t *node_left_right = node_left->right;

    node_left->right = node;
    node->left = node_left_right;

    _avl_update_height(node);
    _avl_update_height(node_left);

    return node_left;
}

static avl_node_t *_avl_rotate_left(avl_node_t *node) {
    assert(node != NULL && node->right != NULL);

    avl_node_t *node_right = node->right;
    avl_node_t *node_right_left = node_right->left;

    node_right->left = node;
    node->right = node_right_left;

    _avl_update_height(node);
    _avl_update_height(node_right);

    return node_right;
}

static avl_node_t *_avl_balance(avl_node_t *node) {
    if (node == NULL) return node;

    _avl_update_height(node);
    int balance = _avl_balance_factor(node);

    // Left Left Case
    if (balance > 1 && _avl_balance_factor(node->left) >= 0) {
        return _avl_rotate_right(node);
    }

    // Left Right Case
    if (balance > 1 && _avl_balance_factor(node->left) < 0) {
        node->left = _avl_rotate_left(node->left);
        return _avl_rotate_right(node);
    }

    // Right Right Case
    if (balance < -1 && _avl_balance_factor(node->right) <= 0) {
        return _avl_rotate_left(node);
    }

    // Right Left Case
    if (balance < -1 && _avl_balance_factor(node->right) > 0) {
        node->right = _avl_rotate_right(node->right);
        return _avl_rotate_left(node);
    }

    return node;
}

static avl_node_t *_avl_insert(avl_node_t *node, int data) {
    if (node == NULL) {
        return _avl_node_new(data);
    }

    if (data < node->data) {
        node->left = _avl_insert(node->left, data);
    } else if (data > node->data) {
        node->right = _avl_insert(node->right, data);
    } else {
        return node;// duplicate
    }

    return _avl_balance(node);
}

static avl_node_t *_avl_delete(avl_node_t *node, int data) {
    if (node == NULL) {
        return node;
    }

    if (data < node->data) {
        node->left = _avl_delete(node->left, data);
    } else if (data > node->data) {
        node->right = _avl_delete(node->right, data);
    } else {
        if (node->left == NULL) {
            avl_node_t *temp = node->right;
            free(node);
            return temp;
        } else if (node->right == NULL) {
            avl_node_t *temp = node->left;
            free(node);
            return temp;
        }

        avl_node_t *temp = node->right;
        while (temp->left != NULL) {
            temp = temp->left;
        }
        node->data = temp->data;
        node->right = _avl_delete(node->right, temp->data);
    }
    return _avl_balance(node);
}

static avl_node_t *_avl_search(avl_node_t *node, int value) {
    if (node == NULL || node->data == value) {
        return node;
    }

    if (value < node->data) {
        return _avl_search(node->left, value);
    } else {
        return _avl_search(node->right, value);
    }

    return NULL;
}

avl_tree_t *avl_new() {
    avl_tree_t *tree = (avl_tree_t *) malloc(sizeof(avl_tree_t));
    if (tree == NULL) {
        return NULL;
    }
    tree->root = NULL;
    return tree;
}

void avl_destroy(avl_tree_t *tree) {
    _avl_destroy_node(tree->root);
    free(tree);
}

void avl_insert(avl_tree_t *tree, int data) {
    assert(tree);
    tree->root = _avl_insert(tree->root, data);
}

void avl_delete(avl_tree_t *tree, int data) {
    assert(tree);
    tree->root = _avl_delete(tree->root, data);
}

avl_node_t *avl_search(avl_tree_t *tree, int data) {
    assert(tree);
    return _avl_search(tree->root, data);
}

void avl_print(avl_node_t *node) {
    if (node != NULL) {
        avl_print(node->left);
        printf("%d\n", node->data);
        avl_print(node->right);
    }
}
