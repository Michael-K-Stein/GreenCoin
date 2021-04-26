
#include "Wallet.h"

void Print_Demo_Keys() {
	DSA_Domain_Parameters * params = Get_Domain_Parameters();
	BN * p = params->p;
	BN * q = params->q;
	BN * G = params->G;

	DSA_Private_Key * priv_key = 0;
	DSA_Public_Key * pub_key = 0;

	DSA_Init_Private_Key(&priv_key);
	DSA_Init_Public_Key(&pub_key);

	DSA_Load_Private_Key(priv_key, p, q, G);
	DSA_Load_Public_Key(pub_key, p, q, G);

	char * buffer;

	DSA_Create_Keys(priv_key, pub_key);
	printf("  x:\r\n");
	TRACE_MPI("    ", (priv_key->x));
	B64_Encode(priv_key->x->data, priv_key->x->size * BN_INT_SIZE, &buffer);
	printf("X B64: '%s'\n", buffer);
	free(buffer);

	printf("  y:\r\n");
	TRACE_MPI("    ", (pub_key->y));
	B64_Encode(pub_key->y->data, pub_key->y->size * BN_INT_SIZE, &buffer);
	printf("Y B64: '%s'\n", buffer);
	free(buffer);
}