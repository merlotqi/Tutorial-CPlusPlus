#include "mpsc.h"

void mpsc_queue_init(mpsc_queue_t *queue, void (*free_callback)(void *data)) {
    node_t *dummy = (node_t *) malloc(sizeof(node_t));
    if (!dummy) {
        return;
    }
    atomic_store(&queue->head, dummy);
    queue->tail = dummy;
    queue->free_callback = free_callback;
}

void mpsc_queue_destroy(mpsc_queue_t *queue) {
    if (!queue) return;

    node_t *current = queue->tail;
    while (current) {
        node_t *next = atomic_load(&current->next);

        if (current->data && queue->free_callback) {
            queue->free_callback(current->data);
        }

        free(current);
        current = next;
    }
}

bool mpsc_queue_enqueue(mpsc_queue_t *queue, void *data) {
    node_t *node = (node_t *) calloc(1, sizeof(node_t));
    if (!node) return false;

    node->data = data;
    atomic_store(&node->next, NULL);

    node_t *prev = atomic_exchange(&queue->head, node);
    atomic_store(&prev->next, node);

    return true;
}

void *mpsc_queue_dequeue(mpsc_queue_t *queue) {
    node_t *tail = queue->tail;
    node_t *next = atomic_load(&tail->next);

    if (next == NULL) return NULL;

    void *data = next->data;
    queue->tail = next;

    if (tail->data && queue->free_callback) {
        queue->free_callback(tail->data);
    }
    free(tail);

    return data;
}

int mpsc_queue_empty(mpsc_queue_t *queue) {
    return atomic_load(&queue->tail->next) == NULL;
}