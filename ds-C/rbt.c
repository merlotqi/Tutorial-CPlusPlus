#include "rbt.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static rbt_node_t *_rbt_node_new(int data) {
    rbt_node_t *node = (rbt_node_t *) malloc(sizeof(rbt_node_t));
    if (node == NULL) {
        return NULL;
    }

    node->data = data;
    node->color = RED;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    return node;
}

static int _rbt_is_black(rbt_node_t *node) {
    return node == NULL || node->color == BLACK;
}

static int _rbt_is_red(rbt_node_t *node) {
    return node != NULL && node->color == RED;
}

static rbt_node_t *_rbt_rotate_right(rbt_node_t *node) {
    assert(node != NULL && node->left != NULL);

    rbt_node_t *node_left = node->left;
    rbt_node_t *node_left_right = node_left->right;

    node_left->right = node;
    node->left = node_left_right;

    node_left->parent = node->parent;
    node->parent = node_left;
    if (node_left_right != NULL) {
        node_left_right->parent = node;
    }

    return node_left;
}

static rbt_node_t *_rbt_rotate_left(rbt_node_t *node) {
    assert(node != NULL && node->right != NULL);

    rbt_node_t *node_right = node->right;
    rbt_node_t *node_right_left = node_right->left;

    node_right->left = node;
    node->right = node_right_left;

    node_right->parent = node->parent;
    node->parent = node_right;
    if (node_right_left != NULL) {
        node_right_left->parent = node;
    }

    return node_right;
}


static rbt_node_t *_rbt_insert(rbt_node_t *node, rbt_node_t *parent, int data, rbt_node_t **new_node) {
    if (node == NULL) {
        *new_node = _rbt_node_new(data);
        if (*new_node == NULL) {
            return NULL;
        }
        (*new_node)->parent = parent;
        return *new_node;
    }

    if (data < node->data) {
        node->left = _rbt_insert(node->left, node, data, new_node);
    } else if (data > node->data) {
        node->right = _rbt_insert(node->right, node, data, new_node);
    }

    return node;
}

static rbt_node_t *_rbt_delete(rbt_node_t *node, int data) {
    if (node == NULL) {
        return NULL;
    }
    return NULL;
}

static rbt_node_t *_rbt_search(rbt_node_t *node, int value) {
    if (node == NULL || node->data == value) {
        return node;
    }

    if (value < node->data) {
        return _rbt_search(node->left, value);
    } else {
        return _rbt_search(node->right, value);
    }

    return NULL;
}

static rbt_node_t *_insert_fixup(rbt_node_t *root, rbt_node_t *node) {
    rbt_node_t *cur = node;

    // case 1: root is black
    if (cur->parent == NULL) {
        cur->color = BLACK;
        return cur;
    }

    // case 2: parent is black
    if (cur->parent->color == BLACK) {
        return root;
    }

    // case 3: parent is red
    while (_rbt_is_red(cur->parent)) {
        rbt_node_t *parent = cur->parent;
        rbt_node_t *grandparent = parent->parent;

        // case 3.1: parent is left child
        if (parent == grandparent->left) {
            rbt_node_t *uncle = grandparent->right;

            // case 3.1.1: uncle is red
            if (_rbt_is_red(uncle)) {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                cur = grandparent;
            } else {
                // left-left case
                if (cur == parent->left) {
                    parent->color = BLACK;
                    grandparent->color = RED;
                    root = _rbt_rotate_right(grandparent);
                } else {
                    // left-right case
                    cur = parent;
                    root = _rbt_rotate_left(parent);

                    parent = cur->parent;
                    grandparent = parent->parent;
                    parent->color = BLACK;
                    grandparent->color = RED;
                    root = _rbt_rotate_right(grandparent);
                }
                break;
            }
        } else {
            rbt_node_t *uncle = grandparent->left;

            // case 3.1.1: uncle is red
            if (_rbt_is_red(uncle)) {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                cur = grandparent;
            } else {
                // right-right case
                if (cur == parent->right) {
                    parent->color = BLACK;
                    grandparent->color = RED;
                    root = _rbt_rotate_left(grandparent);
                } else {
                    // right-left case
                    cur = parent;
                    root = _rbt_rotate_right(parent);

                    parent = cur->parent;
                    grandparent = parent->parent;
                    parent->color = BLACK;
                    grandparent->color = RED;
                    root = _rbt_rotate_left(grandparent);
                }
                break;
            }
        }

        if (cur->parent == NULL) {
            cur->color = BLACK;
            break;
        }
    }

    root->color = BLACK;
    return root;
}

static void _rbt_node_destroy(rbt_node_t *node) {
    if (node == NULL) {
        return;
    }

    _rbt_node_destroy(node->left);
    _rbt_node_destroy(node->right);
    free(node);
}

rbt_t *rbt_new() {
    rbt_t *rbt = (rbt_t *) malloc(sizeof(rbt_t));
    if (rbt == NULL) {
        return NULL;
    }

    rbt->root = NULL;
    return rbt;
}

void rbt_destroy(rbt_t *rbt) {
    assert(rbt);

    _rbt_node_destroy(rbt->root);
    free(rbt);
}

void rbt_insert(rbt_t *rbt, int data) {
    assert(rbt);
    rbt_node_t *new_node = NULL;
    rbt->root = _rbt_insert(rbt->root, NULL, data, &new_node);

    if (new_node != NULL) {
        rbt->root = _insert_fixup(rbt->root, new_node);
    }
}

void rbt_delete(rbt_t *rbt, int data) {
    assert(rbt);

    rbt->root = _rbt_delete(rbt->root, data);
}

rbt_node_t *rbt_search(rbt_t *rbt, int data) {
    assert(rbt);

    return _rbt_search(rbt->root, data);
}

void rbt_print(rbt_node_t *node) {
    if (node == NULL) {
        return;
    }
    rbt_print(node->left);
    printf("%d\n", node->data);
    printf("%s\n", node->color == RED ? "RED" : "BLACK");
    rbt_print(node->right);
}