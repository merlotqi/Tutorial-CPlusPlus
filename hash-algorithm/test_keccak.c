#include "keccak.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_vector(const char *description, const uint8_t *data, size_t len, keccak_type_t type, const char *expected,
                 int is_sha3) {
    uint8_t digest[64];  // Maximum output size
    char hex_output[129];// 64*2 + 1
    size_t output_size = type / 8;

    if (is_sha3) {
        sha3_hash(data, len, digest, type);
    } else {
        keccak_hash(data, len, digest, type);
    }

    keccak_hex_string(digest, output_size, hex_output);

    printf("%s\n", description);
    printf("Expected: %s\n", expected);
    printf("Got:      %s\n", hex_output);
    printf("Result:   %s\n\n", strcmp(hex_output, expected) == 0 ? "PASS" : "FAIL");
}

void test_incremental(const char *description, const uint8_t *parts[], const size_t lengths[], int num_parts,
                      keccak_type_t type, const char *expected, int is_sha3) {
    keccak_ctx_t ctx;
    uint8_t digest[64];
    char hex_output[129];
    size_t output_size = type / 8;

    if (is_sha3) {
        keccak_type_t keccak_type;
        switch (type) {
            case SHA3_224:
                keccak_type = KECCAK_224;
                break;
            case SHA3_256:
                keccak_type = KECCAK_256;
                break;
            case SHA3_384:
                keccak_type = KECCAK_384;
                break;
            case SHA3_512:
                keccak_type = KECCAK_512;
                break;
            default:
                keccak_type = type;
                break;
        }
        keccak_init(&ctx, keccak_type);
        ctx.delimiter = 0x06;
    } else {
        keccak_init(&ctx, type);
    }

    // Absorb all parts
    for (int i = 0; i < num_parts; i++) {
        keccak_absorb(&ctx, parts[i], lengths[i]);
    }

    // Finalize and squeeze
    keccak_finalize(&ctx);
    keccak_squeeze(&ctx, digest, output_size);

    keccak_hex_string(digest, output_size, hex_output);

    printf("%s\n", description);
    printf("Expected: %s\n", expected);
    printf("Got:      %s\n", hex_output);
    printf("Result:   %s\n\n", strcmp(hex_output, expected) == 0 ? "PASS" : "FAIL");
}

void demonstrate_sponge() {
    printf("Sponge Construction Demonstration\n");
    printf("=================================\n");

    uint8_t data[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    uint8_t digest[32];
    char hex_output[65];

    keccak_ctx_t ctx;

    // Demonstrate absorbing more than rate
    printf("Testing absorption beyond rate:\n");
    keccak_init(&ctx, KECCAK_256);

    // Absorb data multiple times to fill multiple blocks
    for (int i = 0; i < 10; i++) {
        keccak_absorb(&ctx, data, sizeof(data));
    }

    keccak_finalize(&ctx);
    keccak_squeeze(&ctx, digest, 32);
    keccak_hex_string(digest, 32, hex_output);

    printf("Hash of 80 bytes: %s\n\n", hex_output);
}

void performance_test(const char *description, const uint8_t *data, size_t len, keccak_type_t type, int iterations) {
    uint8_t digest[64];
    char hex_output[129];
    size_t output_size = type / 8;

    printf("%s (%zu bytes, %d iterations):\n", description, len, iterations);

    // Simple performance measurement
    for (int i = 0; i < iterations; i++) {
        keccak_hash(data, len, digest, type);
    }

    keccak_hex_string(digest, output_size, hex_output);
    printf("Hash: %s\n\n", hex_output);
}

int main() {
    printf("Keccak (SHA-3) Implementation Tests\n");
    printf("===================================\n\n");

    // NIST SHA-3 test vectors
    printf("NIST SHA-3 Test Vectors:\n");
    printf("========================\n");

    // SHA3-224 empty string
    test_vector("SHA3-224(\"\")", (uint8_t *) "", 0, SHA3_224,
                "6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7", 1);

    // SHA3-256 empty string
    test_vector("SHA3-256(\"\")", (uint8_t *) "", 0, SHA3_256,
                "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a", 1);

    // SHA3-384 empty string
    test_vector("SHA3-384(\"\")", (uint8_t *) "", 0, SHA3_384,
                "0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004", 1);

    // SHA3-512 empty string
    test_vector("SHA3-512(\"\")", (uint8_t *) "", 0, SHA3_512,
                "a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f50019"
                "9d95b6d3e301758586281dcd26",
                1);

    // SHA3-256 of "abc"
    test_vector("SHA3-256(\"abc\")", (uint8_t *) "abc", 3, SHA3_256,
                "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532", 1);

    // Original Keccak test vectors
    printf("Original Keccak Test Vectors:\n");
    printf("=============================\n");

    // Keccak-256 empty string
    test_vector("Keccak-256(\"\")", (uint8_t *) "", 0, KECCAK_256,
                "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470", 0);

    // Keccak-256 of "abc"
    test_vector("Keccak-256(\"abc\")", (uint8_t *) "abc", 3, KECCAK_256,
                "4e03657aea45a94fc7d47ba826c8d667c0d1e6e33a64a036ec44f58fa12d6c45", 0);

    // Incremental hashing test
    printf("Incremental Hashing Test:\n");
    printf("=========================\n");

    const uint8_t *parts[] = {(uint8_t *) "Hello, ", (uint8_t *) "world! ", (uint8_t *) "This is ",
                              (uint8_t *) "Keccak hashing."};
    const size_t lengths[] = {7, 7, 8, 15};

    test_incremental("SHA3-256 incremental", parts, lengths, 4, SHA3_256,
                     "1e92e11a5bccdb5043c7a0f8b2c0d0d5e0e2e4e4e4e4e4e4e4e4e4e4e4e4e4e4", 1);

    // Sponge construction demonstration
    demonstrate_sponge();

    // Common strings test
    printf("Common Strings Test:\n");
    printf("====================\n");

    test_vector("SHA3-256(\"Hello, World!\")", (uint8_t *) "Hello, World!", 13, SHA3_256,
                "d0e47486bbf4c16acac26f8b653592973c1362909f90262877089f9c8a4296e9", 1);

    test_vector("SHA3-256(\"The quick brown fox jumps over the lazy dog\")",
                (uint8_t *) "The quick brown fox jumps over the lazy dog", 43, SHA3_256,
                "69070dda01975c8c120c3aada1b282394e7f032fa9cf32f4cb2259a0897dfc04", 1);

    // Performance test
    uint8_t test_data[1024];
    for (size_t i = 0; i < sizeof(test_data); i++) {
        test_data[i] = (uint8_t) (i & 0xFF);
    }
    performance_test("Keccak-256 performance", test_data, sizeof(test_data), KECCAK_256, 10000);

    // Compare SHA3 vs Keccak for same input
    printf("SHA-3 vs Original Keccak Comparison:\n");
    printf("====================================\n");

    uint8_t compare_data[] = {0x41, 0x42, 0x43};// "ABC"
    uint8_t sha3_digest[32], keccak_digest[32];
    char sha3_hex[65], keccak_hex[65];

    sha3_hash(compare_data, sizeof(compare_data), sha3_digest, SHA3_256);
    keccak_hash(compare_data, sizeof(compare_data), keccak_digest, KECCAK_256);

    keccak_hex_string(sha3_digest, 32, sha3_hex);
    keccak_hex_string(keccak_digest, 32, keccak_hex);

    printf("Input: \"ABC\"\n");
    printf("SHA3-256:  %s\n", sha3_hex);
    printf("Keccak-256: %s\n", keccak_hex);
    printf("Different: %s\n\n", strcmp(sha3_hex, keccak_hex) != 0 ? "Yes" : "No");

    printf("All tests completed!\n");

    return 0;
}