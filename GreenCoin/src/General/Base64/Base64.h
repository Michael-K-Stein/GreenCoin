#pragma once

#ifndef __BASE_64_H
#define __BASE_64_H

#include "../General.h"

const char BASE64_CHAR_SEQUENCE[64];
static const signed char BASE64_REVERSE_SEQUENCE[256];

// Returns the size of something to be converted   ~ 4/3 * len
size_t B64_Encoded_Size(size_t len);

// Returns the size of something to be decoded   ~ 3/4 * len
size_t B64_Decoded_Size(char * base64_array);

/* Encode bytes to base64 char array
   Returns the length of the base64 char array.
   Allocates buffer. */
size_t B64_Encode(byte * input, size_t input_length, char ** output);

/* Decodes base64 char array to byte array
   Returns the length of the char array.
   Allocates buffer. */
size_t B64_Decode(char * input, byte ** output);

#endif // !__BASE_64