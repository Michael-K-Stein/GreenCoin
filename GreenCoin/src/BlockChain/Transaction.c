
#include "Transaction.h"

void Print_Transaction_Signature_Part(uint_t * q) {
	for (int i = 4; i >= 0; i--) {
		printf("%.8X ", q[i]);
	}
	printf("\n");
}
void Print_Transaction_Wallet_Address(uint_t * q) {
	for (int i = 31; i >= 0; i--) {
		printf("%.8X ", q[i]);
		if (i % 4 == 0) { printf("\n  "); }
	}
	printf("\n");
}
void Print_Transaction(DSA_Domain_Parameters * params, _Transaction * transaction) {
	printf("=== === === Transaction #%u === === ===\n", transaction->Index);
	printf("\tSender: \n  ");
	Print_Transaction_Wallet_Address(transaction->Sender);
	printf("\tReciever: \n  ");
	Print_Transaction_Wallet_Address(transaction->Reciever);
	printf("\tValue: %.2f GC\n", transaction->Value);
	printf("\tFees: %.4f GC\n", transaction->Fee);
	printf("\tTotal Value: %.4f GC\n", transaction->Value + transaction->Fee);
	printf("\n\t--- Signature ---\n");
	printf("\tr:\n  ");
	Print_Transaction_Signature_Part(transaction->Signature.r);
	printf("\ts:\n  ");
	Print_Transaction_Signature_Part(transaction->Signature.s);

	DSA_Public_Key * pub_key;
	DSA_Init_Public_Key(&pub_key);
	DSA_Load_Public_Key(pub_key, params->p, params->q, params->G);
	BN_Resize(pub_key->y, 32);
	memcpy(pub_key->y->data, transaction->Sender, 32 * sizeof(uint_t));

	DSA_Signature * sign;
	DSA_Init_Signature(&sign);
	BN_Resize(sign->r, 5);
	BN_Resize(sign->s, 5);
	memcpy(sign->r->data, transaction->Signature.r, 5 * sizeof(uint_t));
	memcpy(sign->s->data, transaction->Signature.s, 5 * sizeof(uint_t));

	char * digest = Hash_SHA256(transaction, sizeof(_Transaction) - sizeof(_Signature));

	SIGNATURE_VALID_STATE valid = DSA_Verify_Signature(pub_key, digest, 256, sign);
	if (valid == SIGNATURE_VALID) {
		printf("\tValid signature!\n");
	}
	else {
		printf("\n\t+++ +++ +++ +++ +++\n\tInvalid signature!\n\t+++ +++ +++ +++ +++\n");
	}


	printf("=== === === End Transaction #%u === === ===\n", transaction->Index);
}

void Transaction_Export(FILE * fstream, _Transaction * transaction) {
	fwrite(GCT_MAGIC, 1, 4, fstream);
	fwrite(transaction, sizeof(_Transaction), 1, fstream);
}

void Transaction_Export_To_File(char * file_path, _Transaction * transaction) {
	FILE * ft;
	fopen_s(&ft, file_path, "wb");

	// Write magic
	fwrite(GCT_MAGIC, 1, 4, ft);

	// Write transaction data
	fwrite(transaction, sizeof(_Transaction), 1, ft);
	
	fclose(ft);
}

void Sign_Transaction(DSA_Domain_Parameters * params, _Transaction * transaction, BN * pk) {
	char * transaction_info = (char*)malloc(sizeof(_Transaction) - sizeof(_Signature));
	memcpy(transaction_info, transaction, sizeof(_Transaction) - sizeof(_Signature));

	//char * message = "abc";
	char * message_digest = Hash_SHA256(transaction_info, sizeof(_Transaction) - sizeof(_Signature));//"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";

	DSA_Signature * signature = 0;
	DSA_Init_Signature(&signature);

	DSA_Private_Key * priv_key;
	DSA_Init_Private_Key(&priv_key);
	priv_key->G = params->G;
	priv_key->p = params->p;
	priv_key->q = params->q;
	priv_key->x = pk;

	DSA_Generate_Signature(priv_key, message_digest, 256, signature);

	_Signature sig;
	memcpy((sig.r), signature->r->data, 5 * sizeof(uint_t));
	memcpy((sig.s), signature->s->data, 5 * sizeof(uint_t));

	memcpy(&(transaction->Signature), &sig, sizeof(_Signature));

	free(transaction_info);
	//DSA_Free_Private_Key(priv_key);
}

void Transaction_Demo() {
	DSA_Domain_Parameters * params = Get_Domain_Parameters();

	printf("=== Create Transaction (Demo) ===\n");

	printf("--- Domain Params: ---\n");
	printf("\tp:\r\n");
	TRACE_MPI("\t\t", (params->p));
	printf("\tq:\r\n");
	TRACE_MPI("\t\t", (params->q));
	printf("\tg:\r\n");
	TRACE_MPI("\t\t", (params->G));
	printf("--- --- --- --- --- ---\n");


	_Transaction transaction;

	printf("Transaction #?\n");
	char buffer[64]; fgets(buffer, 64, stdin);
	transaction.Index = strtol(buffer, NULL, 10);

	printf("Please enter your public key (as base64): \n");
	char sender_64[180]; fgets(sender_64, 180, stdin);

	printf("Please enter the reciever's public key (as base64): \n");
	char reciever_64[180]; fgets(reciever_64, 180, stdin);

	printf("How much would you like to send?\n");
	fgets(buffer, 64, stdin);
	double value = strtod(buffer, NULL);
	transaction.Value = value;
	transaction.Fee = 0;

	byte * sender;
	byte * reciever;
	B64_Decode(sender_64, &sender);
	B64_Decode(reciever_64, &reciever);

	memcpy(transaction.Sender, sender, sizeof(_Wallet_Address));
	memcpy(transaction.Reciever, reciever, sizeof(_Wallet_Address));

	printf("Please enter your private key (as base64) in order to sign this transaction: \n");
	char priv_key_64[180]; fgets(priv_key_64, 180, stdin);
	byte * priv_k; size_t priv_key_byte_length = B64_Decode(priv_key_64, &priv_k);
	BN * pk; BN_Init(&pk);
	BN_Resize(pk, 16);
	pk->sign = 1;
	memcpy(pk->data, priv_k, priv_key_byte_length);

	Sign_Transaction(params, &transaction, pk);

	printf("Transaction signed!\n");

	Print_Transaction(params, &transaction);

	char * export_path = "C:\\Users\\stein\\Desktop\\GreenCoin\\Globals\\Test6.GCT";

	printf("Now exporting to: '%s'\n", export_path);

	Transaction_Export_To_File(export_path, &transaction);
}