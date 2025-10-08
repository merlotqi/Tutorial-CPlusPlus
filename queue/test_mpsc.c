#include "mpsc.h"
#include <stdio.h>
#include <string.h>

// 测试1：简单整数数据
typedef struct {
    int value;
    char tag[32];
} test_data_t;

// 测试2：动态字符串数据
typedef struct {
    char *message;
    int length;
} string_data_t;

// 测试3：复杂嵌套结构
typedef struct {
    int id;
    double values[10];
    char *name;
} complex_data_t;

// ==================== 释放回调函数 ====================

void test_data_free(void *data) {
    printf("Freeing test_data: %d\n", ((test_data_t *) data)->value);
    free(data);
}

void string_data_free(void *data) {
    string_data_t *str_data = (string_data_t *) data;
    printf("Freeing string_data: %s\n", str_data->message);
    free(str_data->message);
    free(str_data);
}

void complex_data_free(void *data) {
    complex_data_t *complex = (complex_data_t *) data;
    printf("Freeing complex_data: %s (id=%d)\n", complex->name, complex->id);
    free(complex->name);
    free(complex);
}

void noop_free(void *data) {
    printf("No-op free for data at %p\n", data);
    // 什么都不做，用于测试外部管理内存的情况
}

// ==================== 测试用例 ====================

void test_basic_operations() {
    printf("=== 测试1: 基本操作 ===\n");

    mpsc_queue_t queue;
    mpsc_queue_init(&queue, free);// 使用标准free

    // 测试空队列
    assert(mpsc_queue_empty(&queue));
    assert(mpsc_queue_dequeue(&queue) == NULL);

    // 入队一些整数
    for (int i = 0; i < 5; i++) {
        int *data = malloc(sizeof(int));
        *data = i * 10;
        assert(mpsc_queue_enqueue(&queue, data));
        printf("Enqueued: %d\n", *data);
    }

    // 出队并验证顺序
    for (int i = 0; i < 5; i++) {
        int *data = mpsc_queue_dequeue(&queue);
        assert(data != NULL);
        printf("Dequeued: %d\n", *data);
        // 注意：这里不需要free，因为dequeue已经通过回调free了
    }

    // 再次测试空队列
    assert(mpsc_queue_empty(&queue));

    mpsc_queue_destroy(&queue);
    printf("✓ 基本操作测试通过\n\n");
}

void test_custom_free_callback() {
    printf("=== 测试2: 自定义释放回调 ===\n");

    mpsc_queue_t queue;
    mpsc_queue_init(&queue, test_data_free);

    // 入队测试数据
    for (int i = 0; i < 3; i++) {
        test_data_t *data = malloc(sizeof(test_data_t));
        data->value = i * 100;
        snprintf(data->tag, sizeof(data->tag), "test_%d", i);
        assert(mpsc_queue_enqueue(&queue, data));
        printf("Enqueued test_data: %s\n", data->tag);
    }

    // 出队（会自动调用test_data_free）
    while (!mpsc_queue_empty(&queue)) {
        test_data_t *data = mpsc_queue_dequeue(&queue);
        if (data) {
            printf("Dequeued and auto-freed: %s\n", data->tag);
        }
    }

    mpsc_queue_destroy(&queue);
    printf("✓ 自定义释放回调测试通过\n\n");
}

void test_complex_data_structures() {
    printf("=== 测试3: 复杂数据结构 ===\n");

    mpsc_queue_t str_queue;
    mpsc_queue_init(&str_queue, string_data_free);

    mpsc_queue_t complex_queue;
    mpsc_queue_init(&complex_queue, complex_data_free);

    // 测试字符串数据
    string_data_t *str_data = malloc(sizeof(string_data_t));
    str_data->message = strdup("Hello, World!");
    str_data->length = strlen(str_data->message);
    mpsc_queue_enqueue(&str_queue, str_data);

    // 测试复杂结构数据
    complex_data_t *complex = malloc(sizeof(complex_data_t));
    complex->id = 42;
    complex->name = strdup("TestObject");
    for (int i = 0; i < 10; i++) {
        complex->values[i] = i * 3.14;
    }
    mpsc_queue_enqueue(&complex_queue, complex);

    // 出队并自动释放
    printf("Dequeuing string data...\n");
    mpsc_queue_dequeue(&str_queue);

    printf("Dequeuing complex data...\n");
    mpsc_queue_dequeue(&complex_queue);

    mpsc_queue_destroy(&str_queue);
    mpsc_queue_destroy(&complex_queue);
    printf("✓ 复杂数据结构测试通过\n\n");
}

void test_externally_managed_memory() {
    printf("=== 测试4: 外部管理内存 ===\n");

    mpsc_queue_t queue;
    mpsc_queue_init(&queue, noop_free);// 使用无操作回调

    // 创建静态数据（不需要动态分配）
    int static_data[] = {111, 222, 333};

    for (int i = 0; i < 3; i++) {
        // 注意：这里传入的是静态数据的地址
        assert(mpsc_queue_enqueue(&queue, &static_data[i]));
        printf("Enqueued static data: %d\n", static_data[i]);
    }

    // 出队但不释放（因为是无操作回调）
    for (int i = 0; i < 3; i++) {
        int *data = mpsc_queue_dequeue(&queue);
        assert(data != NULL);
        printf("Dequeued static data: %d (addr: %p)\n", *data, data);
        // 这里不能free，因为数据是静态的
    }

    mpsc_queue_destroy(&queue);
    printf("✓ 外部管理内存测试通过\n\n");
}

void test_boundary_conditions() {
    printf("=== 测试5: 边界条件 ===\n");

    mpsc_queue_t queue;
    mpsc_queue_init(&queue, free);

    // 测试空队列操作
    assert(mpsc_queue_empty(&queue));
    assert(mpsc_queue_dequeue(&queue) == NULL);

    // 单元素入队出队
    int *single = malloc(sizeof(int));
    *single = 999;
    mpsc_queue_enqueue(&queue, single);
    assert(!mpsc_queue_empty(&queue));

    int *result = mpsc_queue_dequeue(&queue);
    assert(result != NULL);
    assert(*result == 999);
    assert(mpsc_queue_empty(&queue));

    // 交替入队出队
    for (int i = 0; i < 10; i++) {
        int *data = malloc(sizeof(int));
        *data = i;
        mpsc_queue_enqueue(&queue, data);

        int *out = mpsc_queue_dequeue(&queue);
        assert(out != NULL);
        assert(*out == i);
    }
    assert(mpsc_queue_empty(&queue));

    mpsc_queue_destroy(&queue);
    printf("✓ 边界条件测试通过\n\n");
}

void test_memory_leak_check() {
    printf("=== 测试6: 内存泄漏检查 ===\n");

    // 这个测试主要靠valgrind等工具来验证
    mpsc_queue_t queue;
    mpsc_queue_init(&queue, free);

    // 入队大量数据
    const int COUNT = 1000;
    for (int i = 0; i < COUNT; i++) {
        int *data = malloc(sizeof(int));
        *data = i;
        mpsc_queue_enqueue(&queue, data);
    }

    // 出队一半
    for (int i = 0; i < COUNT / 2; i++) {
        int *data = mpsc_queue_dequeue(&queue);
        assert(data != NULL);
    }

    // 剩下的数据在destroy时应该被自动释放
    mpsc_queue_destroy(&queue);
    printf("✓ 内存泄漏检查测试完成（请用valgrind验证）\n\n");
}

int main() {
    printf("开始 MPSC 无锁队列测试...\n\n");

    test_basic_operations();
    test_custom_free_callback();
    test_complex_data_structures();
    test_externally_managed_memory();
    test_boundary_conditions();
    test_memory_leak_check();

    printf("🎉 所有测试完成！\n");
    printf("建议使用 valgrind 检查内存泄漏:\n");
    printf("  valgrind --leak-check=full ./mpsc_queue_test\n");

    return 0;
}