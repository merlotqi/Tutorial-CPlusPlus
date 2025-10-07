#ifndef HASH_CRC8_H
#define HASH_CRC8_H

#include <stddef.h>
#include <stdint.h>

// CRC-8 polynomials
typedef enum {
    CRC8_POLY_DEFAULT = 0x07, // CRC-8 (Standard)
    CRC8_POLY_CDMA2000 = 0x9B,// CRC-8 (CDMA2000)
    CRC8_POLY_DARC = 0x39,    // CRC-8 (DARC)
    CRC8_POLY_DVB_S2 = 0xD5,  // CRC-8 (DVB-S2)
    CRC8_POLY_EBU = 0x1D,     // CRC-8 (EBU/CRC-8-AUTOSAR)
    CRC8_POLY_I_CODE = 0x1D,  // CRC-8 (I-CODE)
    CRC8_POLY_ITU = 0x07,     // CRC-8 (ITU)
    CRC8_POLY_MAXIM = 0x31,   // CRC-8 (MAXIM/DOW)
    CRC8_POLY_ROHC = 0x07,    // CRC-8 (ROHC)
    CRC8_POLY_WCDMA = 0x9B    // CRC-8 (WCDMA)
} crc8_poly_t;

// CRC-8 calculation methods
typedef enum {
    CRC8_METHOD_BITWISE,// Bit-by-bit calculation (slow but clear)
    CRC8_METHOD_TABLE   // Lookup table (fast)
} crc8_method_t;

// CRC-8 context structure
typedef struct {
    uint8_t polynomial;    // CRC polynomial
    uint8_t initial_value; // Initial value
    uint8_t final_xor;     // Final XOR value
    uint8_t reflect_input; // Reflect input bytes
    uint8_t reflect_output;// Reflect output CRC
    uint8_t table[256];    // Lookup table (for table method)
    crc8_method_t method;  // Calculation method
} crc8_ctx_t;

// Function prototypes
void crc8_init(crc8_ctx_t *ctx, crc8_poly_t poly, crc8_method_t method);
void crc8_init_custom(crc8_ctx_t *ctx, uint8_t poly, uint8_t init, uint8_t final_xor, uint8_t reflect_in,
                      uint8_t reflect_out);
uint8_t crc8_calculate(const crc8_ctx_t *ctx, const uint8_t *data, size_t len);
uint8_t crc8_update(const crc8_ctx_t *ctx, uint8_t crc, const uint8_t *data, size_t len);

// Utility functions
uint8_t crc8_reflect_byte(uint8_t data);
void crc8_generate_table(crc8_ctx_t *ctx);

// One-shot functions for common polynomials
uint8_t crc8_standard(const uint8_t *data, size_t len);
uint8_t crc8_maxim(const uint8_t *data, size_t len);
uint8_t crc8_dvb_s2(const uint8_t *data, size_t len);

#endif// HASH_CRC8_H