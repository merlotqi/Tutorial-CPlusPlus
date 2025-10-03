#pragma once

typedef struct hash_item_s {
    char *key;
    char *value;
} hash_item_t;

typedef struct hash_table_s {
    int size;
    int count;
    hash_item_t **items;
} hash_table_t;

#define DEFAULT_HT_SIZE (53)
hash_table_t *hash_table_create(const int size);
void hash_table_destory(hash_table_t *ht);

void hash_table_delete(hash_table_t *ht, const char *key);
void hash_table_insert(hash_table_t *ht, const char *key, const char *value);
char *hash_table_search(hash_table_t *ht, const char *key);