//#include "src/BlockChain/BlockChain.h"
//#include "src/ECDSA/Curve/Curve.h"


#include "src/Cryptography/ECDSA/ECDSA.h"
#include "src/Cryptography/DSA/DSA.h"

#include <stdio.h>


int main_c() {

	srand(time(NULL));

	uint p; BN_Init(&p);
	uint q; BN_Init(&q);

	uint_t N = 5;
	uint_t L = 31;

	BN_Set_Value(q, 11);
	DSA_Generate_P(p, q, L);

	uint G; BN_Init(&G);
	uint h; BN_Init(&h);
	BN_Set_Value(h, 2);

	DSA_Generate_G(G, p, q, h);

	DSA_Private_Key * priv_key = 0;
	DSA_Public_Key * pub_key = 0;

	DSA_Init_Private_Key(&priv_key);
	DSA_Init_Public_Key(&pub_key);
	
	DSA_Load_Private_Key(priv_key, p, q, G);
	DSA_Load_Public_Key(pub_key, p, q, G);

	DSA_Create_Keys(priv_key, pub_key);
	TRACE_DEBUG("  x:\r\n");
	TRACE_DEBUG_MPI("    ", (priv_key->x));
	TRACE_DEBUG("  y:\r\n");
	TRACE_DEBUG_MPI("    ", (pub_key->y));

	char * message = "abc";
	char * message_digest = "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
	
	DSA_Signature * signature = 0;
	DSA_Init_Signature(&signature);

	DSA_Generate_Signature(priv_key, message_digest, 256, signature);

	DSA_Verify_Signature(pub_key, message_digest, 256, signature);

	/*
	_Signature * signature = Create_Signature(message, 3, private_key, G, q, p);

	printf("Valid signature: %d\n", Validate_Signature(message, 3, signature, public_key, G, q, p));

	free(p);
	free(q);
	*/
}