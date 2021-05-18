
#include "General.h"

char GCT_MAGIC[4]= { 'G', 'C', 'T', 0x33 }; // GreenCoin Transaction
char GCB_MAGIC[4] = { 'G', 'C', 'B', 0x33 }; // GreenCoin Block



// Returns a pointer to struct DSA_Domain_Parameters
void * Get_Domain_Parameters() {
	uint p; BN_Init(&p);
	uint q; BN_Init(&q);

	uint_t N = 5;
	uint_t L = 31;

	//BN_Set_Value(q, 6071717768788351249);
	//DSA_Generate_P(p, q, L);

	char * q_file_path = "Globals\\q.hex";
	char * p_file_path = "Globals\\p.hex";

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

	BN_Free(h);

	DSA_Domain_Parameters * params = (DSA_Domain_Parameters*)malloc(sizeof(DSA_Domain_Parameters));

	params->G = G;
	params->p = p;
	params->q = q;

	return params;
}