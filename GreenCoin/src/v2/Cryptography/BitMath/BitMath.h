
#pragma once
#ifndef ___BitMath_H
#define ___BitMath_H

#include "../../General/error.h"

typedef unsigned int uint_t;
typedef int int_t;

typedef enum {
	BN_LITTLE_ENDIAN,
	BN_BIG_ENDIA
};

typedef struct {
	int_t sign;		// Positive or negative
	uint_t size;	// Bit size of the number
	uint_t * data;	// The actual bit value of the number
} BN;

void BN_Init(BN * r); // Initialize a bit number;
void BN_Free(BN * r); // Free a bit number;

error_t BN_Resize(BN * n, uint_t size); // Resize the memory capacity of an existing bit number;

uint_t BN_Get_Length(const BN * r);
uint_t BN_Get_Byte_Length(const BN * r);
uint_t BN_Get_Bit_Length(const BN * r);

error_t BN_Set_Bit_Value(BN * r, uint_t index, uint_t value);
uint_t BN_Get_Bit_Value(const BN * r, uint_t index);

int_t BN_Compare(const BN * a, const BN * b);
int_t BN_Compare_Int(const BN * a, int_t b);
int_t BN_Compare_Abs(const BN * a, const BN * b);

error_t BN_Copy(BN * r, const BN * a);
error_t BN_Set_Value(BN * r, int_t a);

error_t BN_Randomize(BN * r, uint_t length);

error_t BN_Import();
error_t BN_Export();

error_t BN_Add(BN * r, const BN * a, const BN * b);				// Adds a and b into r. r = a + b
error_t BN_Add_Int(BN * r, const BN * a, int_t b);				// Addition of Bit number and a normal number
error_t BN_Add_Absolute(BN * r, const BN * a, const BN * b);	// Addition assuming both a and b are positive

error_t BN_Sub(BN * r, const BN * a, const BN * b);				// Subtracts b from a and returns into r. r = a - b
error_t BN_Sub_Int(BN * r, const BN * a, const BN * b);
error_t BN_Sub_Absolute(BN * r, const BN * a, const BN * b);

error_t BN_Shift_Left(BN * r, uint_t l);
error_t BN_Shift_Right(BN * r, uint_t l);

error_t BN_Mul(BN * r, const BN * a, const BN * b);
error_t BN_Mul_Int(BN * r, const BN * a, int_t b);

error_t BN_Div(BN * r, const BN * a, const BN * b);
error_t BN_Div_Int(BN * r, const BN * a, int_t b);

error_t BN_Mod(BN * r, const BN * a, const BN * p);				// r = a % p
error_t BN_Add_Mod(BN * r, const BN * a, const BN * b, const BN * p); // r = (a + b) % p
error_t BN_Sub_Mod(BN * r, const BN * a, const BN * b, const BN * p); // r = (a - b) % p
error_t BN_Mul_Mod(BN * r, const BN * a, const BN * b, const BN * p); // r = (a * b) % p
error_t BN_Exp_Mod(BN * r, const BN * a, const BN * e, const BN * p); // r = pow(a,e) % p
error_t BN_Inv_Mod(BN * r, const BN * a, const BN * p);			// Returns (into r) the multiplicative inverse of a over the field p

#endif // !___BitMath_H
