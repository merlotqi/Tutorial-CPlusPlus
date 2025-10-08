#pragma once

#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct node_s {
    void *data;
    _Atomic(struct node_s *) next;
} node_t;

typedef struct {
    _Atomic(node_t *) head;
    node_t *tail;
    void (*free_callback)(void *data);// optional callback to free data
} mpsc_queue_t;

void mpsc_queue_init(mpsc_queue_t *queue, void (*free_callback)(void *data));
void mpsc_queue_destroy(mpsc_queue_t *queue);

bool mpsc_queue_enqueue(mpsc_queue_t *queue, void *data);
void *mpsc_queue_dequeue(mpsc_queue_t *queue);
int mpsc_queue_empty(mpsc_queue_t *queue);