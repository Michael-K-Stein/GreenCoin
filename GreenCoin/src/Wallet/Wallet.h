#pragma once

#ifndef __WALLET_H
#define __WALLET_H


//#include "../Cryptography/ECDSA/ECDSA.h"
#include "../Cryptography/DSA/DSA.h"
#include "../General/Base64/Base64.h"
#include "../Cryptography/Hash/SHA256.h"

#define ECDSA_SECURITY_LEVEL 128
#define ECDSA_PRIVATE_KEY_LENGTH (ECDSA_SECURITY_LEVEL*2)
#define ECDSA_PRIVATE_KEY_LENGTH_BYTES (ECDSA_PRIVATE_KEY_LENGTH/8)

typedef struct _Public_Keys {
	unsigned char prefix;
	unsigned char x[ECDSA_PRIVATE_KEY_LENGTH_BYTES];
	unsigned char y[ECDSA_PRIVATE_KEY_LENGTH_BYTES];
} _Public_Keys;

typedef uint_t _Wallet_Address[32];

void Print_Demo_Keys();

DSA_Public_Key * Get_Random_Public_Key();

#endif // !__WALLET_H