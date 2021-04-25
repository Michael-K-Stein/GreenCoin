#pragma once

#ifndef __DSA_H
#define __DSA_H

#include "Math/BitMath.h"

typedef uint2048_t p_t;
typedef uint2048_t q_t;
typedef uint2048_t h_t;
typedef uint2048_t g_t;

#pragma region Domain_Parameters

uint16_t L =	2048;
uint16_t N =	256;
/*
uint64_t p;		// Prime modulus. Bit length of p is L.
uint64_t q;		// Prime divisor. Bit length of q is N.
uint64_t g;		// Subgroup genertor, 1 < g < p. The order (n) of the subgroup generated is q % p .

h_t h;			// Counter for random G gereration
*/
#pragma endregion

#pragma region Per_Message_Parameters

uint64_t x;		// Private key. 0 < x < q .
uint64_t y;		// Public key. y = (g^x) % p .
uint64_t k;		// Random number (per messsage). 0 < k < q .
uint64_t k_inv;	// Multiplicative inverse of k with respect to multiplication modulo q. 0 < k_inv < q . (k * k_inv) % q = 1 .

#pragma endregion


g_t Generate_G(p_t p, q_t q);


#endif // !__DSA_H
