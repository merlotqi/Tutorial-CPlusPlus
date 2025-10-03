#include "hash_table.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static hash_item_t HASH_DELETE_ITEM = { NULL, NULL };

static hash_item_t *_hash_item_new(const char *k, const char *v) {
    hash_item_t *i = (hash_item_t *)malloc(sizeof(hash_item_t));
    if (!i) {
        return NULL;
    }

    i->key = strdup(k);
    i->value = strdup(v);
    return i;
}

static void _hash_item_destory(hash_item_t *i)
{
    free(i->key);
    free(i->value);
    free(i);
}

static int _hash(const char *s, const int a, const int m) {
    long hash = 0;
    const int len_s = strlen(s);
    for (size_t i = 0; i < len_s; ++i) {
        hash += (long) pow(a, len_s - (i + 1)) * s[i];
        hash = hash % m;
    }
    return (int) hash;
}

static int _get_hash(const char *s, const int num_buckets, const int attempt) {
    int hash_a = _hash(s, 31, num_buckets);
    int hash_b = _hash(s, 37, num_buckets);
    return (hash_a + attempt * (hash_b == 0 ? 1 : hash_b)) % num_buckets;
}

static void _hash_table_resize(hash_table_t *ht, const int to_size) {
    if (to_size < 50) {
        return;
    }

    hash_table_t *new_ht = hash_table_create(to_size);
    if (!new_ht) {
        fprintf(stderr, "new a hash table error\n");
        exit(-1);
    }

    for(int i = 0; i < ht->size; ++i) {
        hash_item_t *item = ht->items[i];
        if (item != NULL && item != &HASH_DELETE_ITEM) {
            hash_table_insert(new_ht, item->key, item->value);
        }
    }

    hash_table_t *tmp = ht;
    ht = new_ht;
    new_ht = tmp;

    hash_table_destory(new_ht);
}

static void _hash_table_resize_down(hash_table_t *ht) {
    const int new_size = ht->size / 2;
    _hash_table_resize(ht, new_size);
}

static void _hash_table_resize_up(hash_table_t *ht) {
    const int new_size = ht->size * 2;
    _hash_table_resize(ht, new_size);
}


hash_table_t *hash_table_create(const int size) {
    hash_table_t *ht = (hash_table_t *)malloc(sizeof(hash_table_t));
    if (!ht) {
        return NULL;
    }
    ht->size = size;
    ht->count = 0;
    ht->items = (hash_item_t **)calloc((size_t)ht->size, sizeof(hash_item_t *));
    return ht;
}

void hash_table_destory(hash_table_t *ht)
{
    for (int i = 0; i < ht->size; i++) {
        hash_item_t *item = ht->items[i];
        if (item != NULL && item != &HASH_DELETE_ITEM) {
            _hash_item_destory(item);
        }
    }
    free(ht->items);
    free(ht);
}

void hash_table_delete(hash_table_t *ht, const char *key) {
    const int load = ht->count * 100 / ht->size;
    if (load < 10) {
        _hash_table_resize_down(ht);
    }

    int index = _get_hash(key, ht->size, 0);
    hash_item_t *item = ht->items[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HASH_DELETE_ITEM) {
            if (strcmp(item->key, key) == 0) {
                _hash_item_destory(item);
                ht->items[index] = &HASH_DELETE_ITEM;
                ht->count--;
                return;
            }
        }
        index = _get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }
}

void hash_table_insert(hash_table_t *ht, const char *key, const char *value) {
    const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        _hash_table_resize_up(ht);
    }

    hash_item_t *item = _hash_item_new(key, value);

    int index = _get_hash(item->key, ht->size, 0);
    hash_item_t *cur = ht->items[index];
    int i = 1;
    while (cur != NULL) {
        if (cur != &HASH_DELETE_ITEM) {
            if (strcmp(item->key, cur->key) == 0) {
                _hash_item_destory(cur);
                ht->items[index] = item;
                return;
            }
        }
        index = _get_hash(item->key, ht->size, i);
        cur = ht->items[index];
        i++;
    }

    ht->items[index] = item;
    ht->count++;
}

char *hash_table_search(hash_table_t *ht, const char *key) {
    int index = _get_hash(key, ht->size, 0);
    hash_item_t *item = ht->items[index];
    int i = 1;
    while (item) {
        if (item != &HASH_DELETE_ITEM) {
            if (strcmp(item->key, key) == 0) {
                return item->value;
            }
        }
        index = _get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }
    return NULL;
}

#ifdef TEST


#endif