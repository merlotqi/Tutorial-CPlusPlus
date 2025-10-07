#include "crc8.h"
#include <stdio.h>
#include <string.h>

void print_binary(uint8_t value) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
    }
}

void test_crc8(const char *test_name, const uint8_t *data, size_t len, crc8_poly_t poly, uint8_t expected) {
    crc8_ctx_t ctx;

    // Test both methods
    for (int method = CRC8_METHOD_BITWISE; method <= CRC8_METHOD_TABLE; method++) {
        crc8_init(&ctx, poly, (crc8_method_t) method);
        uint8_t result = crc8_calculate(&ctx, data, len);

        const char *method_name = (method == CRC8_METHOD_BITWISE) ? "Bitwise" : "Table";

        printf("%s - %s: ", test_name, method_name);
        printf("CRC = 0x%02X", result);

        if (result == expected) {
            printf(" ✓ PASS\n");
        } else {
            printf(" ✗ FAIL (Expected: 0x%02X)\n", expected);
        }
    }
    printf("\n");
}

void demonstrate_polynomials() {
    uint8_t test_data[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};// "123456789"
    size_t len = sizeof(test_data);

    printf("CRC-8 Polynomial Comparison for data: ");
    for (size_t i = 0; i < len; i++) {
        printf("%c", test_data[i]);
    }
    printf("\n");
    printf("================================================================\n\n");

    // Test various polynomials
    struct {
        crc8_poly_t poly;
        const char *name;
        uint8_t expected;
    } tests[] = {
            { CRC8_POLY_DEFAULT,    "CRC-8 (Standard)", 0xF4},
            {CRC8_POLY_CDMA2000,    "CRC-8 (CDMA2000)", 0xDA},
            {    CRC8_POLY_DARC,        "CRC-8 (DARC)", 0x15},
            {  CRC8_POLY_DVB_S2,      "CRC-8 (DVB-S2)", 0xBC},
            {     CRC8_POLY_EBU, "CRC-8 (EBU/AUTOSAR)", 0x97},
            {   CRC8_POLY_MAXIM,       "CRC-8 (MAXIM)", 0xA1},
            {   CRC8_POLY_WCDMA,       "CRC-8 (WCDMA)", 0x25}
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        crc8_ctx_t ctx;
        crc8_init(&ctx, tests[i].poly, CRC8_METHOD_TABLE);
        uint8_t result = crc8_calculate(&ctx, test_data, len);

        printf("%-20s: 0x%02X", tests[i].name, result);
        if (result == tests[i].expected) {
            printf(" ✓\n");
        } else {
            printf(" ✗ (Expected: 0x%02X)\n", tests[i].expected);
        }
    }
    printf("\n");
}

void test_streaming() {
    printf("Streaming CRC-8 Calculation Test\n");
    printf("================================\n");

    uint8_t data1[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F};      // "Hello"
    uint8_t data2[] = {0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64};// " World"
    uint8_t data3[] = {0x21};                              // "!"

    crc8_ctx_t ctx;
    crc8_init(&ctx, CRC8_POLY_DEFAULT, CRC8_METHOD_TABLE);

    // Calculate CRC in one shot for comparison
    uint8_t combined[12];
    memcpy(combined, data1, sizeof(data1));
    memcpy(combined + sizeof(data1), data2, sizeof(data2));
    memcpy(combined + sizeof(data1) + sizeof(data2), data3, sizeof(data3));

    uint8_t crc_one_shot = crc8_calculate(&ctx, combined, sizeof(combined));

    // Calculate CRC using streaming
    uint8_t crc_stream = ctx.initial_value;
    crc_stream = crc8_update(&ctx, crc_stream, data1, sizeof(data1));
    crc_stream = crc8_update(&ctx, crc_stream, data2, sizeof(data2));
    crc_stream = crc8_update(&ctx, crc_stream, data3, sizeof(data3));

    // Apply final reflection and XOR for streaming result
    if (ctx.reflect_output) {
        crc_stream = crc8_reflect_byte(crc_stream);
    }
    crc_stream ^= ctx.final_xor;

    printf("One-shot CRC: 0x%02X\n", crc_one_shot);
    printf("Streaming CRC: 0x%02X\n", crc_stream);
    printf("Result: %s\n\n", (crc_one_shot == crc_stream) ? "PASS" : "FAIL");
}

void demonstrate_reflection() {
    printf("Byte Reflection Demonstration\n");
    printf("=============================\n");

    uint8_t test_bytes[] = {0x01, 0x81, 0x42, 0xFF, 0xAA};

    for (size_t i = 0; i < sizeof(test_bytes); i++) {
        uint8_t original = test_bytes[i];
        uint8_t reflected = crc8_reflect_byte(original);

        printf("0x%02X: ", original);
        print_binary(original);
        printf(" -> ");
        print_binary(reflected);
        printf(" (0x%02X)\n", reflected);
    }
    printf("\n");
}

int main() {
    printf("CRC-8 Implementation Tests\n");
    printf("==========================\n\n");

    // Test 1: Empty data
    test_crc8("Empty data", NULL, 0, CRC8_POLY_DEFAULT, 0x00);

    // Test 2: Single byte
    uint8_t single_byte[] = {0x61};// 'a'
    test_crc8("Single byte", single_byte, 1, CRC8_POLY_DEFAULT, 0xE8);

    // Test 3: Simple string
    uint8_t hello[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F};// "Hello"
    test_crc8("String 'Hello'", hello, sizeof(hello), CRC8_POLY_DEFAULT, 0x92);

    // Test 4: One-shot functions
    printf("One-shot Function Tests:\n");
    uint8_t test_data[] = {0x31, 0x32, 0x33, 0x34, 0x35};// "12345"

    printf("CRC-8 Standard: 0x%02X\n", crc8_standard(test_data, sizeof(test_data)));
    printf("CRC-8 MAXIM: 0x%02X\n", crc8_maxim(test_data, sizeof(test_data)));
    printf("CRC-8 DVB-S2: 0x%02X\n\n", crc8_dvb_s2(test_data, sizeof(test_data)));

    // Demonstrate different polynomials
    demonstrate_polynomials();

    // Test streaming capability
    test_streaming();

    // Demonstrate byte reflection
    demonstrate_reflection();

    return 0;
}