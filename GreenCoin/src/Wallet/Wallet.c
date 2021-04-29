
#include "Wallet.h"

#include "../BlockChain/BlockChain.h"

double Calculate_Wallet_Value(char * dir_path, _Wallet_Address pk, uint64_t up_to_block_index) {
	
	double value = 0;

	char fp[256] = { 0 };
	char magic[4] = { 0 };
	
	_Block block;

	for (uint64_t i = 0; i < up_to_block_index; i++) {
			
		sprintf_s(fp, 256, "%s\\%llu.GCB", dir_path, i);

		FILE * f;
		fopen_s(&f, fp, "rb");

		if (f == NULL) { break; }

		fread(magic, sizeof(char), 4, f);

		if (memcmp(magic, GCB_MAGIC, 4)) { printf("Malformatted file in directory!\n'%s' is invalid!\n", fp); break; }

			
		fread(&block, sizeof(char), sizeof(_Block), f);

		fclose(f);

		for (int t = 0; t < MAXIMUM_AMOUNT_OF_TRANSACTIONS_ON_LEDGER; t++) {
			_Transaction * transaction = &(block.Transactions[t]);

			value += Calculate_Transaction_Change_To_Wallet(transaction, pk);
		}

		if (memcmp(block.Notary_Address, pk, sizeof(_Wallet_Address)) == 0) {
			value += Calculate_Block_Total_Miner_Fees(&block);
		}
	}

	return value;
}

void Print_Demo_Keys() {
	DSA_Domain_Parameters * params = Get_Domain_Parameters();
	BN * p = params->p;
	BN * q = params->q;
	BN * G = params->G;

	DSA_Private_Key * priv_key = 0;
	DSA_Public_Key * pub_key = 0;

	DSA_Init_Private_Key(&priv_key);
	DSA_Init_Public_Key(&pub_key);

	//DSA_Load_Private_Key(priv_key, p, q, G);
	//DSA_Load_Public_Key(pub_key, p, q, G);

	char * buffer;

	DSA_Create_Keys(params, priv_key, pub_key);
	printf("  x:\r\n");
	TRACE_MPI("    ", (priv_key));
	B64_Encode(priv_key->data, priv_key->size * BN_INT_SIZE, &buffer);
	printf("X B64: '%s'\n", buffer);
	free(buffer);

	printf("  y:\r\n");
	TRACE_MPI("    ", (pub_key));
	B64_Encode(pub_key->data, pub_key->size * BN_INT_SIZE, &buffer);
	printf("Y B64: '%s'\n", buffer);
	free(buffer);
}

DSA_Public_Key * Get_Random_Public_Key() {
	DSA_Domain_Parameters * params = Get_Domain_Parameters();
	BN * p = params->p;
	BN * q = params->q;
	BN * G = params->G;

	DSA_Private_Key * priv_key = 0;
	DSA_Public_Key * pub_key = 0;

	DSA_Init_Private_Key(&priv_key);
	DSA_Init_Public_Key(&pub_key);

	//DSA_Load_Private_Key(priv_key, p, q, G);
	//DSA_Load_Public_Key(pub_key, p, q, G);

	char * buffer;

	DSA_Create_Keys(params, priv_key, pub_key);
	/*printf("  x:\r\n");
	TRACE_MPI("    ", (priv_key->x));
	B64_Encode(priv_key->x->data, priv_key->x->size * BN_INT_SIZE, &buffer);
	printf("X B64: '%s'\n", buffer);
	free(buffer);

	printf("  y:\r\n");
	TRACE_MPI("    ", (pub_key->y));
	B64_Encode(pub_key->y->data, pub_key->y->size * BN_INT_SIZE, &buffer);
	printf("Y B64: '%s'\n", buffer);
	free(buffer);*/

	return pub_key;
}