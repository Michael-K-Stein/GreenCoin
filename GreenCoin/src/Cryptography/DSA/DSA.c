
#include "DSA.h"

void DSA_Init_Public_Key(DSA_Public_Key ** key) {
	BN_Init(key);
	/**key = (DSA_Public_Key*)malloc(sizeof(DSA_Public_Key));
	DSA_Public_Key * key_addr = *key;
	/*BN_Init(&(key_addr->p));
	BN_Init(&(key_addr->q));
	BN_Init(&(key_addr->G));
	BN_Init(&(key_addr->y));*/
}
/*void DSA_Load_Public_Key_From_Params(DSA_Public_Key ** key, DSA_Domain_Parameters * params) {
	*key = (DSA_Public_Key*)malloc(sizeof(DSA_Public_Key));
	DSA_Public_Key * key_addr = *key;
	key_addr->p = params->p;
	key_addr->q = params->q;
	key_addr->G = params->G;
	BN_Init(&(key_addr->y));
}
void DSA_Load_Public_Key_Domain_Params(DSA_Public_Key * key, DSA_Domain_Parameters * params) {
	if (key->p != NULL) { memcpy(key->p, params->p, params->p->size + 2); }
	else {
		key->p = params->p;
	}
	if (key->q != NULL) { memcpy(key->q, params->q, params->q->size + 2); }
	else {
		key->q = params->q;
	}
	if (key->G != NULL) { memcpy(key->G, params->G, params->G->size + 2); }
	else {
		key->G = params->G;
	}	
}*/
void DSA_Free_Public_Key(DSA_Public_Key * key) {
	/*BN_Free(&(key->p));
	BN_Free(&(key->q));
	BN_Free(&(key->G));
	BN_Free(&(key->y));*/
	BN_Free(key);
}
/*void DSA_Load_Public_Key(DSA_Public_Key * key, uint p, uint q, uint G) {
	key->p = p;
	key->q = q;
	key->G = G;
}*/

void DSA_Init_Private_Key(DSA_Private_Key ** key) {
	BN_Init(key);
	/*
	*key = (DSA_Private_Key*)malloc(sizeof(DSA_Private_Key));
	DSA_Private_Key * key_addr = *key;
	BN_Init(&(key_addr->p));
	BN_Init(&(key_addr->q));
	BN_Init(&(key_addr->G));
	BN_Init(&(key_addr->x));*/
}
void DSA_Free_Private_Key(DSA_Private_Key * key) {
	/*BN_Free(&(key->p));
	BN_Free(&(key->q));
	BN_Free(&(key->G));
	BN_Free(&(key->x));*/
	BN_Free(key);
}
/*void DSA_Load_Private_Key(DSA_Private_Key * key, uint p, uint q, uint G) {
	key->p = p;
	key->q = q;
	key->G = G;
}*/

error_t DSA_Create_Keys(DSA_Domain_Parameters * params, DSA_Private_Key * priv_key, DSA_Public_Key * pub_key) {
	BN * p;
	BN * q;
	BN * G;
	/*if (BN_Compare(priv_key->p, pub_key->p) != 0) { return ERROR_FAILED; }
	if (BN_Compare(priv_key->q, pub_key->q) != 0) { return ERROR_FAILED; }
	if (BN_Compare(priv_key->G, pub_key->G) != 0) { return ERROR_FAILED; }*/

	p = params->p;
	q = params->q;
	G = params->G;

	uint_t N;
	uint_t L;

	N = BN_Get_Bit_Length(q);
	L = BN_Get_Bit_Length(p);

	BN c1;
	BN c;
	BN_Init_Stack(&c1);
	BN_Init_Stack(&c);

	BN_Randomize(&c1, N + 64);

	BN qt;
	BN_Init_Stack(&qt);
	BN_Sub_Int(&qt, q, 1);

	BN_Mod(&c, &c1, &qt);
	BN_Add_Int(priv_key, &c, 1);

	BN_Exp_Mod(pub_key, G, priv_key, p);

	free(c1.data);
	free(c.data);

	return ERROR_NONE;
}

void DSA_Init_Signature(DSA_Signature ** signature) {
	*signature = (DSA_Signature*)malloc(sizeof(signature));
	DSA_Signature * sig_addr = *signature;
	BN_Init(&(sig_addr->r));
	BN_Init(&(sig_addr->s));
}
void DSA_Free_Signature(DSA_Signature * signature) {
	BN_Free((signature->r));
	BN_Free((signature->s));
	//free(signature);
}

void DSA_Generate_P(uint p, uint q, uint_t L) {
	BN_Add_Int(p, q, 1);

	int_t m = 2;

	while (!BN_Is_Prime(p)) {
		BN_Mul_Int(p, q, m);
		BN_Add_Int(p, p, 1);
		m++;
	}
}

void DSA_Generate_G(uint G, uint p, uint q, uint h) {
	BN * e;
	BN_Init(&e);
	BN * r;
	BN_Init(&r);

	BN * t; BN_Init(&t);
	BN_Sub_Int(t, p, 1);

	BN_Div(e, r, t, q);

	do {
		BN_Exp_Mod(G, h, e, p);
		BN_Add_Int(h, h, 1);
	} while (!BN_Compare_Int(G, 1));

	BN_Free(e);
	BN_Free(t);
	BN_Free(r);
}

error_t DSA_Generate_Signature(DSA_Domain_Parameters * params, const DSA_Private_Key * key, const uint8_t * message_digest, size_t message_len, DSA_Signature * signature) {
	error_t error = 0;
	uint_t n;
	BN k;
	BN z;

	//Check parameters
	if (key == NULL || message_digest == NULL || signature == NULL) {
		return ERROR_FAILED;
	}

	//Debug message
	TRACE_DEBUG("DSA signature generation...\r\n");
	TRACE_DEBUG("  p:\r\n");
	TRACE_DEBUG_MPI("    ", (key->p));
	TRACE_DEBUG("  q:\r\n");
	TRACE_DEBUG_MPI("    ", (key->q));
	TRACE_DEBUG("  g:\r\n");
	TRACE_DEBUG_MPI("    ", (key->G));
	TRACE_DEBUG("  x:\r\n");
	TRACE_DEBUG_MPI("    ", (key->x));
	TRACE_DEBUG("  digest:\r\n");
	TRACE_DEBUG_ARRAY("    ", message_digest, message_len);

	//Initialize multiple precision integers
	BN_Init_Stack(&k);
	BN_Init_Stack(&z);

	//Let N be the bit length of q
	n = BN_Get_Bit_Length((params->q));

	//Compute N = MIN(N, outlen)
	n = min(n, message_len * 4);

	//Convert the digest to a multiple precision integer
	BN_Import_Hex_String(&z, message_digest, (n + 3) / 4, BN_BIG_ENDIAN);

	//Keep the leftmost N bits of the hash value
	if ((n % 8) != 0)
	{
		BN_Shift_Right(&z, 8 - (n % 8));
	}

	//Debug message
	TRACE_DEBUG("  z:\r\n");
	TRACE_DEBUG_MPI("    ", &z);


	do {

		//Generated a pseudorandom number
		BN_Randomize(&k, n);

		//Make sure that 0 < k < q
		if (BN_Compare(&k, (params->q)) >= 0)
			BN_Shift_Right(&k, 1);

		//Compute r = (g ^ k mod p) mod q
		BN_Exp_Mod((signature->r), (params->G), &k, (params->p));
		BN_Mod((signature->r), (signature->r), (params->q));

		//Compute k ^ -1 mod q
		BN_Inv_Mod(&k, &k, params->q);

		//Compute s = k ^ -1 * (z + x * r) mod q
		BN_Mul((signature->s), (key), (signature->r));
		BN_Add((signature->s), (signature->s), &z);
		//BN_Mod((signature->s), (signature->s), (key->q));
		BN_Mul_Mod((signature->s), (signature->s), &k, (params->q));
	} while ((BN_Compare_Int(signature->r, 0) == 0 || BN_Compare_Int(signature->s, 0) == 0));

	//Dump DSA signature
	TRACE_DEBUG("  r:\r\n");
	TRACE_DEBUG_MPI("    ", (signature->r));
	TRACE_DEBUG("  s:\r\n");
	TRACE_DEBUG_MPI("    ", (signature->s));

	free(k.data);
	free(z.data);

	//Clean up side effects if necessary
	if (error)
	{
		//Release (R, S) integer pair
		BN_Free((signature->r));
		BN_Free((signature->s));
	}

	return error;
}
error_t DSA_Verify_Signature(DSA_Domain_Parameters * params, const DSA_Public_Key * key, const uint8_t * message_digest, size_t message_len, DSA_Signature * signature) {

	BN * r = signature->r;
	BN * s = signature->s;

	BN * p = params->p;
	BN * q = params->q;
	BN * G = params->G;
	BN * y = key;

	if (!(BN_Compare_Int(r, 0) > 0 && BN_Compare(r, q) < 0)) { return SIGNATURE_INVALID; }
	if (!(BN_Compare_Int(s, 0) > 0 && BN_Compare(s, q) < 0)) { return SIGNATURE_INVALID; }

	BN w;
	BN z;
	BN u1;
	BN u2;
	BN v;
	BN_Init_Stack(&w);
	BN_Init_Stack(&z);
	BN_Init_Stack(&u1);
	BN_Init_Stack(&u2);
	BN_Init_Stack(&v);

	BN_Inv_Mod(&w, s, q);


	uint_t n = BN_Get_Bit_Length(q);
	n = min(n, message_len * 4);
	BN_Import_Hex_String(&z, message_digest, (n + 3) / 4, BN_BIG_ENDIAN);
	if ((n % 8) != 0)
	{
		BN_Shift_Right(&z, 8 - (n % 8));
	}


	BN_Mul_Mod(&u1, &z, &w, q);
	BN_Mul_Mod(&u2, r, &w, q);

	BN tmp1;
	BN tmp2;
	BN_Init_Stack(&tmp1);
	BN_Init_Stack(&tmp2);

	BN_Exp_Mod(&tmp1, G, &u1, p);
	BN_Exp_Mod(&tmp2, y, &u2, p);

	BN_Mul_Mod(&v, &tmp1, &tmp2, p);
	BN_Mod(&v, &v, q);

	free(tmp1.data);
	free(tmp2.data);

	TRACE_DEBUG("  v:\r\n");
	TRACE_DEBUG_MPI("    ", &v);

	int valid = (BN_Compare(&v, r) == 0);

	free(w.data);
	free(z.data);
	free(u1.data);
	free(u2.data);
	free(v.data);

	if (valid) {
		return SIGNATURE_VALID;
	}

	return SIGNATURE_INVALID;
}