#ifndef HASH_CRC32_H
#define HASH_CRC32_H

#include <stddef.h>
#include <stdint.h>

// CRC-32 polynomials
typedef enum {
    CRC32_POLY_IEEE_8023 = 0xEDB88320, // IEEE 802.3 (Ethernet, ZIP, PNG)
    CRC32_POLY_CASTAGNOLI = 0x82F63B78,// Castagnoli (iSCSI, Btrfs, ext4)
    CRC32_POLY_KOOPMAN = 0xEB31D82E,   // Koopman
    CRC32_POLY_Q = 0xD5828281          // CRC-32Q (aviation, AIXM)
} crc32_poly_t;

// CRC-32 calculation methods
typedef enum {
    CRC32_METHOD_BITWISE,   // Bit-by-bit calculation
    CRC32_METHOD_TABLE,     // Lookup table (8-bit)
    CRC32_METHOD_TABLE_16BIT// Lookup table (16-bit, faster)
} crc32_method_t;

// CRC-32 context structure
typedef struct {
    uint32_t polynomial;           // CRC polynomial
    uint32_t initial_value;        // Initial value
    uint32_t final_xor;            // Final XOR value
    uint8_t reflect_input;         // Reflect input bytes
    uint8_t reflect_output;        // Reflect output CRC
    uint32_t table[256];           // Lookup table (for table methods)
    uint32_t table_16bit[256][256];// 16-bit lookup table
    crc32_method_t method;         // Calculation method
} crc32_ctx_t;

// Function prototypes
void crc32_init(crc32_ctx_t *ctx, crc32_poly_t poly, crc32_method_t method);
void crc32_init_custom(crc32_ctx_t *ctx, uint32_t poly, uint32_t init, uint32_t final_xor, uint8_t reflect_in,
                       uint8_t reflect_out);
uint32_t crc32_calculate(const crc32_ctx_t *ctx, const uint8_t *data, size_t len);
uint32_t crc32_update(const crc32_ctx_t *ctx, uint32_t crc, const uint8_t *data, size_t len);

// Utility functions
uint32_t crc32_reflect(uint32_t data, int bits);
uint8_t crc32_reflect_byte(uint8_t data);
void crc32_generate_table(crc32_ctx_t *ctx);
void crc32_generate_16bit_table(crc32_ctx_t *ctx);

// One-shot functions for common polynomials
uint32_t crc32_ieee(const uint8_t *data, size_t len);
uint32_t crc32_castagnoli(const uint8_t *data, size_t len);
uint32_t crc32_koopman(const uint8_t *data, size_t len);

#endif// HASH_CRC32_H