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

	//BN_Set_Value(q, 6071717768788351249);
	//DSA_Generate_P(p, q, L);

	char * q_file_path = "C:\\Users\\stein\\Desktop\\GreenCoin\\Globals\\q.hex";
	char * p_file_path = "C:\\Users\\stein\\Desktop\\GreenCoin\\Globals\\p.hex";

	FILE * fq;
	FILE * fp;
	fopen_s(&fq, q_file_path, "rb");
	fopen_s(&fp, p_file_path, "rb");

	char q_bin[20];
	char p_bin[128];

	fread(q_bin, sizeof(char), 20, fq);
	fread(p_bin, sizeof(char), 128, fp);

	fclose(fq);
	fclose(fp);

	BN_Import(q, q_bin, 20, BN_BIG_ENDIAN);
	BN_Import(p, p_bin, 128, BN_BIG_ENDIAN);


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

	SIGNATURE_VALID_STATE valid = DSA_Verify_Signature(pub_key, message_digest, 256, signature);
	if (valid == SIGNATURE_VALID) {
		printf("Valid signature!\n");
	}
	else {
		printf("Invalid signature!\n");
	}
}