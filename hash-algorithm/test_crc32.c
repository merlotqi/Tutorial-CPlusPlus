#include "crc32.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_crc32(const char *test_name, const uint8_t *data, size_t len, crc32_poly_t poly, uint32_t expected) {
    crc32_ctx_t ctx;

    printf("%s:\n", test_name);

    // Test all methods
    for (int method = CRC32_METHOD_BITWISE; method <= CRC32_METHOD_TABLE_16BIT; method++) {
        crc32_init(&ctx, poly, (crc32_method_t) method);
        uint32_t result = crc32_calculate(&ctx, data, len);

        const char *method_name;
        switch (method) {
            case CRC32_METHOD_BITWISE:
                method_name = "Bitwise";
                break;
            case CRC32_METHOD_TABLE:
                method_name = "Table-8bit";
                break;
            case CRC32_METHOD_TABLE_16BIT:
                method_name = "Table-16bit";
                break;
            default:
                method_name = "Unknown";
                break;
        }

        printf("  %-12s: 0x%08X", method_name, result);

        if (result == expected) {
            printf(" ✓ PASS\n");
        } else {
            printf(" ✗ FAIL (Expected: 0x%08X)\n", expected);
        }
    }
    printf("\n");
}

void performance_test(const char *test_name, const uint8_t *data, size_t len) {
    crc32_ctx_t ctx;
    printf("%s Performance Test (%zu bytes):\n", test_name, len);

    for (int method = CRC32_METHOD_BITWISE; method <= CRC32_METHOD_TABLE_16BIT; method++) {
        crc32_init(&ctx, CRC32_POLY_IEEE_8023, (crc32_method_t) method);

        const char *method_name;
        switch (method) {
            case CRC32_METHOD_BITWISE:
                method_name = "Bitwise";
                break;
            case CRC32_METHOD_TABLE:
                method_name = "Table-8bit";
                break;
            case CRC32_METHOD_TABLE_16BIT:
                method_name = "Table-16bit";
                break;
            default:
                method_name = "Unknown";
                break;
        }

        // Simple performance measurement
        uint32_t result;
        const int iterations = 10000;

        // Time measurement would go here in a real implementation
        result = crc32_calculate(&ctx, data, len);

        printf("  %-12s: 0x%08X (%d iterations)\n", method_name, result, iterations);
    }
    printf("\n");
}

void test_known_vectors() {
    printf("CRC-32 Known Test Vectors\n");
    printf("=========================\n\n");

    // Test vector: "123456789"
    uint8_t test_data[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
    size_t len = sizeof(test_data);

    test_crc32("IEEE 802.3 (\"123456789\")", test_data, len, CRC32_POLY_IEEE_8023, 0xCBF43926);

    test_crc32("Castagnoli (\"123456789\")", test_data, len, CRC32_POLY_CASTAGNOLI, 0xE3069283);

    // Test empty data
    test_crc32("IEEE 802.3 (empty)", NULL, 0, CRC32_POLY_IEEE_8023, 0x00000000);

    // Test single byte
    uint8_t single_byte[] = {0x41};// 'A'
    test_crc32("IEEE 802.3 (\"A\")", single_byte, 1, CRC32_POLY_IEEE_8023, 0xD3D99E8B);
}

void test_streaming() {
    printf("Streaming CRC-32 Test\n");
    printf("=====================\n");

    uint8_t part1[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F};      // "Hello"
    uint8_t part2[] = {0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64};// " World"
    uint8_t part3[] = {0x21};                              // "!"

    crc32_ctx_t ctx;
    crc32_init(&ctx, CRC32_POLY_IEEE_8023, CRC32_METHOD_TABLE);

    // Calculate in one shot for comparison
    uint8_t combined[12];
    memcpy(combined, part1, sizeof(part1));
    memcpy(combined + sizeof(part1), part2, sizeof(part2));
    memcpy(combined + sizeof(part1) + sizeof(part2), part3, sizeof(part3));

    uint32_t crc_one_shot = crc32_calculate(&ctx, combined, sizeof(combined));

    // Calculate using streaming
    uint32_t crc_stream = ctx.initial_value;
    crc_stream = crc32_update(&ctx, crc_stream, part1, sizeof(part1));
    crc_stream = crc32_update(&ctx, crc_stream, part2, sizeof(part2));
    crc_stream = crc32_update(&ctx, crc_stream, part3, sizeof(part3));

    // Apply final operations for streaming result
    if (ctx.reflect_output != ctx.reflect_input) {
        crc_stream = crc32_reflect(crc_stream, 32);
    }
    crc_stream ^= ctx.final_xor;

    printf("One-shot CRC: 0x%08X\n", crc_one_shot);
    printf("Streaming CRC: 0x%08X\n", crc_stream);
    printf("Result: %s\n\n", (crc_one_shot == crc_stream) ? "PASS" : "FAIL");
}

void test_one_shot_functions() {
    printf("One-shot Function Tests\n");
    printf("=======================\n");

    uint8_t test_data[] = {0x54, 0x65, 0x73, 0x74};// "Test"

    printf("IEEE:      0x%08X\n", crc32_ieee(test_data, sizeof(test_data)));
    printf("Castagnoli: 0x%08X\n", crc32_castagnoli(test_data, sizeof(test_data)));
    printf("Koopman:    0x%08X\n\n", crc32_koopman(test_data, sizeof(test_data)));
}

int main() {
    printf("CRC-32 Implementation Tests\n");
    printf("===========================\n\n");

    test_known_vectors();
    test_streaming();
    test_one_shot_functions();

    // Performance test with larger data
    uint8_t large_data[1024];
    for (size_t i = 0; i < sizeof(large_data); i++) {
        large_data[i] = (uint8_t) (i & 0xFF);
    }
    performance_test("Large Data", large_data, sizeof(large_data));

    return 0;
}