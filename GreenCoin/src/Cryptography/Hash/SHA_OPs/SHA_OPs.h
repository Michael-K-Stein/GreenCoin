#pragma once

#include <stdint.h>
#include <string.h>

uint32_t SHA256_op_ch(uint32_t x, uint32_t y, uint32_t z);

uint32_t SHA256_op_maj(uint32_t x, uint32_t y, uint32_t z);

uint32_t SHA256_op_a(uint32_t x);

uint32_t SHA256_op_b(uint32_t x);

uint32_t SHA256_op_c(uint32_t x);

uint32_t SHA256_op_d(uint32_t x);