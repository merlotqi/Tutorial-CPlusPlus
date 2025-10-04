#include "rbt.h"

int main() {
    rbt_t *rbt = rbt_new();

    rbt_insert(rbt, 17);
    rbt_insert(rbt, 18);
    rbt_insert(rbt, 23);
    rbt_insert(rbt, 34);
    rbt_insert(rbt, 27);
    rbt_insert(rbt, 15);
    rbt_insert(rbt, 9);
    rbt_insert(rbt, 6);
    rbt_insert(rbt, 8);
    rbt_insert(rbt, 5);
    rbt_insert(rbt, 25);

    rbt_print(rbt->root);

    rbt_destroy(rbt);

    return 0;
}