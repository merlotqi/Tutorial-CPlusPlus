#include "hash_table.h"

#include <stdio.h>

void test_insert(hash_table_t *ht) {
    hash_table_insert(ht, "10", "afaf");
    hash_table_insert(ht, "20", "afaefer");
    hash_table_insert(ht, "20", "afaxxxefer");
    hash_table_insert(ht, "30", "4rafcag");
    hash_table_insert(ht, "40", "afsb fer");
    hash_table_insert(ht, "50", "aACsxefer");
    hash_table_insert(ht, "60", "4rfdng");
}

void test_search(hash_table_t *ht) {
    printf("%s\n", hash_table_search(ht, "40"));
    printf("%s\n", hash_table_search(ht, "20"));
}

void test_delete(hash_table_t *ht) {
    hash_table_delete(ht, "30");
}

void test_output(hash_table_t *ht) {
    printf("size %d, count %d\n", ht->size, ht->count);
    for (int i = 0; i < ht->size; i++) {
        if ((i + 1) % 5 == 0) {
            printf("\n");
        }
        hash_item_t *item = ht->items[i];
        if (item == NULL) {
            printf("(:)\t\t\t");
        } else {
            printf("(%s:%s)\t\t\t", ht->items[i]->key, ht->items[i]->value);
        }
    }
}

int main() {
    hash_table_t *ht = hash_table_create(50);

    test_insert(ht);
    test_search(ht);
    test_delete(ht);
    test_output(ht);

    hash_table_destory(ht);
    return 0;
}