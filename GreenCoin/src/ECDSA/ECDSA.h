#pragma once

#ifndef __ECDSA_H
#define __ECDSA_H


//#include "SHA256.h"
#include "DSA/Math/BitMath.h"
//#include "Curve/Curve.h"
#include <time.h>

#define SHA256_LENGTH_IN_BYTES (256/8)

typedef struct _Signature _Signature;
struct _Signature {
	uint64_t r;
	uint64_t s;
};

_Signature * Create_Signature(void * message, size_t message_len, uint64_t private_key, uint2048_t G, uint2048_t q, uint2048_t p);
int Validate_Signature(void * message, size_t message_len, _Signature * signature, uint2048_t public_key, uint2048_t G, uint2048_t q, uint2048_t p);
/*void Create_Signature(void * message, size_t message_len, int private_key) {
	/*int n = 1 % 7;
	uint64_t d = private_key;

	char * e_tmp = Hash_SHA256((char*)message, message_len);
	uint256_t e; memcpy(e.bits, e_tmp, SHA256_LENGTH_IN_BYTES);

	uint256_t z;
	// Take n leftmost bits of e
	int op_n = 256 - n;
	for (int i = 0; i < 4; i++) {
		if (op_n > 256 - ((i + 1) * 64)) {
			uint64_t xr = 0b11111111111111111111111111111111;
			xr = xr >> (op_n % 64);

			z.bits[i] = e.bits[i] ^ xr;
		}
	}

	int r = 0;
	int s = 0;
	while (r == 0 || s == 0){
		// Select random number k
		srand(time(NULL) + e.bits[0]);
		uint32_t k = (rand() + 1) % (n - 1);

		// Calculate curve point
		double x, y = k * G;

		r = x % n;

		int k_inv = 0;
		while ((k_inv * k) % n != 1) { k_inv++; }

		s = (k_inv * (z + r * d)) % n;
	}

	printf("Keys are: r, s: %d, %d\n", r, s);

	
}*/

#endif // !__ECDSA_H