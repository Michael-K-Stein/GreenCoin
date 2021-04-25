
#include "ECDSA.h"
#define min(a,b) (a>b)?b:a
/*
_Signature * Create_Signature(void * message, size_t message_len, uint64_t private_key, BN * G_b, BN * q_b, BN * p_b) {

	uint64_t p = (*p_b)[0];
	uint64_t q = (*q_b)[0];
	uint64_t G = (*G_b)[0];

	uint64_t x = private_key;

	int n = ceil(log2(q+1));

	uint64_t k = 0;
	uint64_t k_inv = 0;

	uint64_t r = 0;
	uint64_t s = 0;
	while (s == 0 || r == 0) {

		k = 19;// 1 + (rand() % (n - 1));
		while ((k_inv * k) % q != 1) { k_inv++; }

		uint2048_t k_b = Init(k);
		uint2048_t tmp = Init(0);
		Pow(G_b, k_b, p_b, tmp);

		r = (*tmp)[0] % q;
		free(tmp);
		free(k_b);
		// Valid up to here

		char * e_tmp = "cb1ad2119d8fafb69566510ee712661f9f14b83385006ef92aec47f523a38358";// Hash_SHA256((char*)message, message_len);
		uint256_t e;
		SHA256_to_uint256(e_tmp, &e);
		//memcpy(e.bits, e_tmp, SHA256_LENGTH_IN_BYTES);

		int outlen = 0;
		int ind = 0;
		while (e.bits[ind] == 0) { ind++; }
		outlen = ((3 - ind) * 64) + log2(e.bits[ind]);

		uint64_t z = 0;
		memcpy(&z, e.bits, (int)ceil(min(n, outlen)/8));

		s = (k_inv * (z + (x*r))) % q;
	}

	printf("(r, s) : (0x%.16llX, 0x%.16llX)\n", r, s);

	_Signature * signature = (_Signature*)malloc(sizeof(_Signature));
	signature->r = r;
	signature->s = s;
	return signature;
}

int Validate_Signature(void * message, size_t message_len, _Signature * signature, BN * public_key, BN * G_b, BN * q_b, BN * p_b) {

	uint64_t p = (*p_b)[0];
	uint64_t q = (*q_b)[0];
	uint64_t G = (*G_b)[0];

	uint64_t r = signature->r;
	uint64_t s = signature->s;

	uint64_t y = (*public_key)[0];

	int n = ceil(log2(q + 1));

	if (!(0 < r && r < q && 0 < s && s < q)) { return 0; }

	uint64_t w = Inv_64(s, q);


	char * e_tmp = "cb1ad2119d8fafb69566510ee712661f9f14b83385006ef92aec47f523a38358";// Hash_SHA256((char*)message, message_len);
	uint256_t e;
	SHA256_to_uint256(e_tmp, &e);
	int outlen = 0;
	int ind = 0;
	while (e.bits[ind] == 0) { ind++; }
	outlen = ((3 - ind) * 64) + log2(e.bits[ind]);

	uint64_t z = 0;
	memcpy(&z, e.bits, (int)ceil(min(n, outlen) / 8));

	uint2048_t u1 = Init((z * w) % q);
	uint2048_t u2 = Init((r * w) % q);

	uint2048_t tmp1 = Init(0); Pow(G_b, u1, q_b, tmp1);
	uint2048_t tmp2 = Init(0); Pow(public_key, u2, q_b, tmp2);

	uint2048_t v = Init(0); Mul(tmp1, tmp2, p_b, v);
	Mod(v, q_b, v);

	return ((*v)[0] == r);
}*/