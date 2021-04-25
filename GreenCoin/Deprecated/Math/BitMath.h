#pragma once

#ifndef __BitMath_H
#define __BitMath_H

#include <stdint.h>
#include <math.h>
#include <string.h>

/*
typedef uint64_t uint2048_ptr_t[32];
typedef uint2048_ptr_t* uint2048_t;
*/

typedef uint64_t uint2048_ptr_t[1];
typedef uint2048_ptr_t* uint2048_t;

typedef struct uint256_t uint256_t;
struct uint256_t
{
	uint64_t bits[4];
};

uint2048_t	Init(uint64_t a);		// Normal num to special
uint2048_t	Init_from_val(uint2048_t a);
void		Dinit(uint2048_t a);	// Deallocate
void		Print_Hex(uint2048_t a);

int			Equ(uint2048_t a, uint2048_t b);	// Equals. Returns (a==b)

void	Add(uint2048_t a, uint2048_t b, uint2048_t out);
void	Sub(uint2048_t a, uint2048_t b, uint2048_t out);
void	Mul(uint2048_t a, uint2048_t b, uint2048_t modu, uint2048_t out);
void	Div(uint2048_t a, uint2048_t b, uint2048_t out);
void	Pow(uint2048_t a, uint2048_t b, uint2048_t modu, uint2048_t out);
void	Mod(uint2048_t a, uint2048_t b, uint2048_t out);
uint64_t Inv_64(uint64_t a, uint64_t modu);
void	Inv(uint2048_t a, uint2048_t modu, uint2048_t out);

void		Inc(uint2048_t a); // In place +1
void		Dec(uint2048_t a); // In place -1

#endif // !__BitMath_H
