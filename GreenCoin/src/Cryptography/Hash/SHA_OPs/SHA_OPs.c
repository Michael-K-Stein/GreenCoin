
#include "SHA_OPs.h"

uint32_t SHA256_op_ch(uint32_t x, uint32_t y, uint32_t z)
{
	return ((x & y) ^ (~x & z));
}

uint32_t SHA256_op_maj(uint32_t x, uint32_t y, uint32_t z)
{
	return ((x & y) ^ (x & z) ^ (y & z));
}

uint32_t SHA256_op_a(uint32_t x)
{
	return (bit_rotate_right(x, 2) ^ bit_rotate_right(x, 13) ^ bit_rotate_right(x, 22));
}

uint32_t SHA256_op_b(uint32_t x)
{
	return (bit_rotate_right(x, 6) ^ bit_rotate_right(x, 11) ^ bit_rotate_right(x, 25));
}

uint32_t SHA256_op_c(uint32_t x)
{
	return (bit_rotate_right(x, 7) ^ bit_rotate_right(x, 18) ^ (x >> 3));
}

uint32_t SHA256_op_d(uint32_t x)
{
	return (bit_rotate_right(x, 17) ^ bit_rotate_right(x, 19) ^ (x >> 10));
}