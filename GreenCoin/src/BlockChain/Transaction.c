
#include "Transaction.h"

void Print_Transaction_Signature_Part(FILE * fstream, uint_t * q) {
	for (int i = 4; i >= 0; i--) {
		fprintf(fstream, "%.8X ", q[i]);
	}
	fprintf(fstream, "\n");
}
void Print_Transaction_Wallet_Address(FILE * fstream, uint_t * q) {
	fprintf(fstream, "  ");
	for (int i = 31; i >= 0; i--) {
		fprintf(fstream, "%.8X ", q[i]);
		if (i % 4 == 0) { fprintf(fstream, "\n  "); }
	}
	fprintf(fstream, "\n");
}
void Print_Transaction(FILE * fstream, DSA_Domain_Parameters * params, _Transaction * transaction) {
	fprintf(fstream, "=== === === Transaction #%u === === ===\n", transaction->Index);
	fprintf(fstream, "\tSender: \n");
	Print_Transaction_Wallet_Address(fstream, transaction->Sender);
	fprintf(fstream, "\tReciever: \n");
	Print_Transaction_Wallet_Address(fstream, transaction->Reciever);
	fprintf(fstream, "\tValue: %.2f GC\n", transaction->Value);
	fprintf(fstream, "\tFees: %.4f GC\n", transaction->Fee);
	fprintf(fstream, "\tTotal Value: %.4f GC\n", transaction->Value + transaction->Fee);
	fprintf(fstream, "\n\t--- Signature ---\n");
	fprintf(fstream, "\tr:\n  ");
	Print_Transaction_Signature_Part(fstream, transaction->Signature.r);
	fprintf(fstream, "\ts:\n  ");
	Print_Transaction_Signature_Part(fstream, transaction->Signature.s);

	DSA_Public_Key * pub_key=0;
	DSA_Init_Public_Key(&pub_key);
	//DSA_Load_Public_Key(pub_key, params->p, params->q, params->G);
	BN_Resize(pub_key, 32);
	memcpy(pub_key->data, transaction->Sender, 32 * sizeof(uint_t));

	DSA_Signature * sign;
	DSA_Init_Signature(&sign);
	BN_Resize(sign->r, 5);
	BN_Resize(sign->s, 5);
	memcpy(sign->r->data, transaction->Signature.r, 5 * sizeof(uint_t));
	memcpy(sign->s->data, transaction->Signature.s, 5 * sizeof(uint_t));

	char * digest = Hash_SHA256(transaction, sizeof(_Transaction) - sizeof(_Signature));

	SIGNATURE_VALID_STATE valid = DSA_Verify_Signature(params, pub_key, digest, 64, sign);
	if (valid == SIGNATURE_VALID) {
		fprintf(fstream, "\tValid signature!\n");
	}
	else {
		fprintf(fstream, "\n\t+++ +++ +++ +++ +++\n\tInvalid signature!\n\t+++ +++ +++ +++ +++\n");
	}


	fprintf(fstream, "=== === === End Transaction #%u === === ===\n", transaction->Index);
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
	priv_key = pk;

	DSA_Generate_Signature(params, priv_key, message_digest, 64, signature);

	_Signature sig;
	memcpy((sig.r), signature->r->data, 5 * sizeof(uint_t));
	memcpy((sig.s), signature->s->data, 5 * sizeof(uint_t));

	memcpy(&(transaction->Signature), &sig, sizeof(_Signature));

	free(transaction_info);
	//DSA_Free_Private_Key(priv_key);
}

SIGNATURE_VALID_STATE Verify_Transaction(DSA_Domain_Parameters * params, _Transaction * transaction) {
	DSA_Public_Key * pub_key = 0;
	DSA_Init_Public_Key(&pub_key);
	BN_Resize(pub_key, 32);
	memcpy(pub_key->data, transaction->Sender, 32 * sizeof(uint_t));

	DSA_Signature * sign;
	DSA_Init_Signature(&sign);
	BN_Resize(sign->r, 5);
	BN_Resize(sign->s, 5);
	memcpy(sign->r->data, transaction->Signature.r, 5 * sizeof(uint_t));
	memcpy(sign->s->data, transaction->Signature.s, 5 * sizeof(uint_t));

	char * digest = Hash_SHA256(transaction, sizeof(_Transaction) - sizeof(_Signature));

	SIGNATURE_VALID_STATE valid = DSA_Verify_Signature(params, pub_key, digest, 64, sign);

	DSA_Free_Public_Key(pub_key);
	DSA_Free_Signature(sign);

	return valid;
}

double Calculate_Transaction_Change_To_Wallet(_Transaction * transaction, _Wallet_Address pk) {
	if (memcmp(transaction->Sender, pk, 32 * sizeof(uint_t)) == 0) {
		return -(transaction->Value + transaction->Fee);
	}
	else if (memcmp(transaction->Reciever, pk, 32 * sizeof(uint_t)) == 0) {
		return transaction->Value;
	}
	else {
		return 0;
	}
	return 0;
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

	printf("Block #?\n");
	char buffer[64]; fgets(buffer, 64, stdin);
	transaction.Block_Index = strtol(buffer, NULL, 10);

	printf("Transaction #?\n");
	fgets(buffer, 64, stdin);
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

	printf("\n\n\n");
	printf("Transaction summary: \n");

	Print_Transaction(stderr, params, &transaction);
	printf("\n\n\n");

	printf("Type 'EXECUTE' to continue, otherwise cancel the transaction.\n");

	char * EXECUTE = "EXECUTE";
	fgets(buffer, 64, stdin); buffer[strlen(EXECUTE)] = 0x0;

	if (strcmp(buffer, EXECUTE) == 0) {
		printf("Transaction executed!\n");

		char export_path[256];// = "C:\\Users\\stein\\Desktop\\GreenCoin\\Globals\\Test6.GCT";
		sprintf_s(export_path, 256, "C:\\Users\\stein\\Desktop\\GreenCoin\\Globals\\Demo_Transactions\\Test_%u.GCT", transaction.Index);

		printf("Now exporting to: '%s'\n", export_path);

		Transaction_Export_To_File(export_path, &transaction);
	}
	else {
		printf("Transaction has been cancelled!\n");
	}
}