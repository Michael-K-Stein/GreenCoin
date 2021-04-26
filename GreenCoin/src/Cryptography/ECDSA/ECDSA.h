#pragma once

#ifndef __ECDSA_H
#define __ECDSA_H


#include "Curve/Curve.h"
#include <time.h>

#define SHA256_LENGTH_IN_BYTES (256/8)

typedef struct _Signature _Signature;
struct _Signature {
	BN r;
	BN s;
};

_Signature * Create_Signature(void * message, size_t message_len, uint64_t private_key, BN * G, BN * q, BN * p);
int Validate_Signature(void * message, size_t message_len, _Signature * signature, BN * public_key, BN * G, BN * q, BN * p);

#endif // !__ECDSA_H