#pragma once

#ifndef __SHA256_H
#define __SHA256_H

#include <stdint.h>
#include <string.h>

#include "SHA_Rotate/SHA_Rotate.h"
#include "SHA_OPs/SHA_OPs.h"

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

const uint32_t g_SHA256_k[64];

const uint32_t g_SHA256_default_buffers[8];

// Format a message into a standard 512bit block
size_t format_message(char * message, size_t message_len, unsigned char ** output_message_ptr);

uint32_t bit_swap_32(uint32_t x);

uint64_t bit_swap_64(uint64_t x);

char * SHA256_strncpy(char *dst, const char *src, size_t n);

void uint32_arr_assign_add(uint32_t *dst, const uint32_t *src, size_t len);
void uint32_arr_cpy(uint32_t *dst, const uint32_t *src, size_t len);
void uint64_arr_assign_add(uint64_t *dst, const uint64_t *src, size_t len);
void uint64_arr_cpy(uint64_t *dst, const uint64_t *src, size_t len);



char * uitoa_base(uintmax_t nb, intmax_t base, char letter);
char * uitoa_base_len(uintmax_t nb, intmax_t base, char letter, size_t len);

void SHA256_init_w_array(t_64_uint32 w_array, unsigned char *formatted_msg);

void SHA256_shuffle_buffers(t_8_uint32 buffers, t_64_uint32 w_array);
void SHA256_run_ops(t_8_uint32 buffers, unsigned char *formatted_msg, size_t msg_len);

char * build_hash(uint32_t *buffers, size_t buffer_count);
// The SHA-256 hash algorithm
char * Hash_SHA256(char * message, size_t message_len);

#endif // !__SHA256_H