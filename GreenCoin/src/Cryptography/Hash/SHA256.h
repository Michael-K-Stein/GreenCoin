#pragma once

#include <stdint.h>
#include <string.h>

typedef uint32_t	t_4_uint32[4];
typedef uint32_t	t_8_uint32[8];
typedef uint64_t	t_8_uint64[8];
typedef uint32_t	t_16_uint32[16];
typedef uint32_t	t_64_uint32[64];
typedef uint64_t	t_80_uint32[80];
typedef uint64_t	t_80_uint64[80];

// Format a message into a standard 512bit block
size_t format_message(char * message, size_t message_len, unsigned char ** output_message_ptr);

#define DEC(x) (x-1)

// Chunks of 512 bits
#define SHA256_CHUNK_SIZE 64

#define SHA256_CHUNKS_SIZE(len) ((len + 1 + 8 + DEC(SHA256_CHUNK_SIZE)) & -DEC(SHA256_CHUNK_SIZE))
#define SHA256_CHUNK_COUNT(len) (SHA256_CHUNKS_SIZE(len) / SHA256_CHUNK_SIZE)

const uint32_t g_sha256_k[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
	0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
	0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
	0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
	0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
	0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

const uint32_t g_sha256_default_buffers[8] = {
	0x6a09e667,
	0xbb67ae85,
	0x3c6ef372,
	0xa54ff53a,
	0x510e527f,
	0x9b05688c,
	0x1f83d9ab,
	0x5be0cd19
};


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