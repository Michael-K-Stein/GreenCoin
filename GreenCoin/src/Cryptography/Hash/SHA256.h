#pragma once

#ifndef __SHA256_H
#define __SHA256_H

#include <stdint.h>
#include <string.h>

typedef uint32_t	t_4_uint32[4];
typedef uint32_t	t_8_uint32[8];
typedef uint64_t	t_8_uint64[8];
typedef uint32_t	t_16_uint32[16];
typedef uint32_t	t_64_uint32[64];
typedef uint64_t	t_80_uint32[80];
typedef uint64_t	t_80_uint64[80];

#define DEC(x) (x-1)

// Chunks of 512 bits
#define SHA256_CHUNK_SIZE 64

#define SHA256_CHUNKS_SIZE(len) ((len + 1 + 8 + DEC(SHA256_CHUNK_SIZE)) & -DEC(SHA256_CHUNK_SIZE))
#define SHA256_CHUNK_COUNT(len) (SHA256_CHUNKS_SIZE(len) / SHA256_CHUNK_SIZE)

const uint32_t g_sha256_k[64];

const uint32_t g_sha256_default_buffers[8];

// Format a message into a standard 512bit block
size_t format_message(char * message, size_t message_len, unsigned char ** output_message_ptr);

char * ft_sha256(const char *msg, size_t msg_len);

uint32_t ft_bswap_uint32(uint32_t x);

uint64_t ft_bswap_uint64(uint64_t x);

char	*ft_strncpy(char *dst, const char *src, size_t n);

void ft_uint32_arr_assign_add(uint32_t *dst, const uint32_t *src, size_t len);
void ft_uint32_arr_cpy(uint32_t *dst, const uint32_t *src, size_t len);
void ft_uint64_arr_assign_add(uint64_t *dst, const uint64_t *src, size_t len);
void ft_uint64_arr_cpy(uint64_t *dst, const uint64_t *src, size_t len);

uint32_t		ft_brotate_left(uint32_t x, char rotations);
uint32_t		ft_brotate_right(uint32_t x, char rotations);
uint64_t		ft_brotate_right_64(uint64_t x, char rotations);

uint32_t	sha256_op_ch(uint32_t x, uint32_t y, uint32_t z);

uint32_t sha256_op_maj(uint32_t x, uint32_t y, uint32_t z);

uint32_t sha256_op_a(uint32_t x);

uint32_t sha256_op_b(uint32_t x);

uint32_t sha256_op_c(uint32_t x);

uint32_t sha256_op_d(uint32_t x);
char	*ft_uitoa_base(uintmax_t nb, intmax_t base, char letter);
char	*ft_uitoa_base_len(uintmax_t nb, intmax_t base, char letter, size_t len);
void sha256_init_w_array(t_64_uint32 w_array, unsigned char *formatted_msg);

void sha256_shuffle_buffers(t_8_uint32 buffers, t_64_uint32 w_array);
void	sha256_run_ops(t_8_uint32 buffers, unsigned char *formatted_msg, size_t msg_len);
char * build_hash(uint32_t *buffers, size_t buffer_count);
// The SHA-256 hash algorithm
char * Hash_SHA256(char * message, size_t message_len);
/*
void SHA256_to_uint256(char * inp, uint256_t * out) {
	int hash_len = 64;

	for (int i = 0; i < 4; i++) {

		int hash_part_length = hash_len / 4; // This gives us a part of 256 / 4 = 64 bits
		uint64_t part = 0;

		for (int hex_ind = 0; hex_ind < hash_part_length; hex_ind++) {

			char c = inp[(i*hash_part_length) + hex_ind]; // Start at leftmost and move right

			uint64_t dig = strtol(&c, NULL, 16);
			uint64_t exp = pow(16, hash_part_length - hex_ind - 1);

			part += dig * exp;

		}

		out->bits[i] = part;

	}
}*/

#endif // !__SHA256_H