
#pragma once
#ifndef ___DSA_H
#define ___DSA_H

#include "../BNMath/BNMath.h"
#include "../../General/debug.h"

typedef BN* uint;

typedef struct {
	uint p; // Prime modulus
	uint q; // Order of the group
	uint G; // Group generator - The point on the Eliptic Curve
} DSA_Domain_Parameters;

typedef struct {
	uint p;
	uint q;
	uint G;
	uint x; // Secret part of private key
} DSA_Private_Key;

typedef struct {
	uint p;
	uint q;
	uint G;
	uint y;
} DSA_Public_Key;

typedef struct {
	uint r;
	uint s;
} DSA_Signature;

typedef enum {
	SIGNATURE_VALID,
	SIGNATURE_INVALID
} SIGNATURE_VALID_STATE;

void DSA_Init_Public_Key(DSA_Public_Key * key);
void DSA_Free_Public_Key(DSA_Public_Key * key);
void DSA_Load_Public_Key(DSA_Public_Key * key, uint p, uint q, uint G);

void DSA_Init_Private_Key(DSA_Private_Key * key);
void DSA_Free_Private_Key(DSA_Private_Key * key);
void DSA_Load_Private_Key(DSA_Private_Key * key, uint p, uint q, uint G);

error_t DSA_Create_Keys(DSA_Private_Key * priv_key, DSA_Public_Key * pub_key);

void DSA_Init_Signature(DSA_Signature * signature);
void DSA_Free_Signature(DSA_Signature * signature);

void DSA_Generate_P(uint p, uint q, uint_t L);
void DSA_Generate_G(uint G, uint p, uint q, uint h);

error_t DSA_Generate_Signature(const DSA_Private_Key * key, const uint8_t * message_digest, size_t message_len, DSA_Signature * signature);
SIGNATURE_VALID_STATE DSA_Verify_Signature(const DSA_Public_Key * key, const uint8_t * message_digest, size_t message_len, DSA_Signature * signature);

#endif // !___DSA_H
