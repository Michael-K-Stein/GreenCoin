
#include "Wallet.h"

#include "../BlockChain/BlockChain.h"

command_t WALLET_COMMAND_HELP = {
	"help",
	"Displays help menu.",
	Wallet_Help
};
command_t WALLET_COMMAND_GENERATE_WALLET = {
	"generate",
	"Generates new wallet credentials.",
	Print_Demo_Keys
};
command_t WALLET_COMMAND_WALLET_VALUE = {
	"value",
	"Calculates the value of a wallet.",
	Wallet_Calculate_Value_CommandLine
};
command_t WALLET_COMMAND_ALL_VALUES = {
	"values",
	"Prints all wallet values.",
	Wallet_Calculate_Values
};
command_t WALLET_COMMAND_EXIT = {
	"exit",
	"Exits the wallet command-line.",
	Wallet_Exit
};

command_t * WALLET_COMMANDS[5] = { &WALLET_COMMAND_HELP, &WALLET_COMMAND_GENERATE_WALLET, &WALLET_COMMAND_WALLET_VALUE, &WALLET_COMMAND_ALL_VALUES, &WALLET_COMMAND_EXIT };


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

struct Wallet_Piece {
	_Wallet_Address address;
	void * next;
};
int Wallet_Chain_Contains_Address(struct Wallet_Piece * wallet_chain, _Wallet_Address wallet_address) {

	struct Wallet_Piece * ptr = wallet_chain;

	while (ptr->next != NULL) {
		ptr = ptr->next;

		if (memcmp(ptr->address, wallet_address, sizeof(_Wallet_Address)) == 0) {
			return 1;
		}
	}

	return 0;
}
int Wallet_Add_Address_To_Chain(struct Wallet_Piece * wallet_chain, _Wallet_Address wallet_address) {
	struct Wallet_Piece * ptr = wallet_chain;

	while (ptr->next != NULL) {
		ptr = ptr->next;
	}

	ptr->next = (struct Wallet_Piece *)calloc(1, sizeof(struct Wallet_Piece));
	memcpy(((struct Wallet_Piece *)(ptr->next))->address, wallet_address, sizeof(_Wallet_Address));
}
int Wallet_Chain_Recursive_Free(struct Wallet_Piece * wallet_chain) {
	if (wallet_chain->next != NULL) {
		Wallet_Chain_Recursive_Free(wallet_chain->next);
	}
	free(wallet_chain);
	return 0;
}
void Wallet_Calculate_Values() {
	double coins_in_circulation = (double)INITIAL_BLOCK_MINING_FEE;
	coins_in_circulation *= (pow((double)MINING_FEE_DECAY, BlockChainLength) - 1);
	coins_in_circulation /= ((double)MINING_FEE_DECAY - 1);

	printf("There are currently %.20f GreenCoins in circulation.\n", coins_in_circulation);

	struct Wallet_Piece * wallet_chain = (struct Wallet_Piece*)calloc(1, sizeof(struct Wallet_Piece));

	_Block b;
	_Wallet_Address wallet_address;
	char * address_base64_buffer;
	FILE * f;
	for (uint64_t block_ind = 0; block_ind < BlockChainLength; block_ind++) {
		error_t err = Open_Block_File(&f, block_ind);
		if (err != ERROR_NONE) { break; }
		fseek(f, 4, SEEK_SET);
		fread(&b, sizeof(_Block), 1, f);
		fclose(f);

		for (int trans_ind = 0; trans_ind < MAXIMUM_AMOUNT_OF_TRANSACTIONS_ON_LEDGER; trans_ind++) {
			memcpy(wallet_address, b.Transactions[trans_ind].Sender, sizeof(_Wallet_Address));

			if (!Wallet_Chain_Contains_Address(wallet_chain, wallet_address)) {
				Wallet_Add_Address_To_Chain(wallet_chain, wallet_address);
			}

			memcpy(wallet_address, b.Transactions[trans_ind].Reciever, sizeof(_Wallet_Address));

			if (!Wallet_Chain_Contains_Address(wallet_chain, wallet_address)) {
				Wallet_Add_Address_To_Chain(wallet_chain, wallet_address);
			}
		}

		memcpy(wallet_address, b.Notary_Address, sizeof(_Wallet_Address));

		if (!Wallet_Chain_Contains_Address(wallet_chain, wallet_address)) {
			Wallet_Add_Address_To_Chain(wallet_chain, wallet_address);
		}
	}

	struct Wallet_Piece * ptr = wallet_chain;
	while (ptr->next != NULL) {
		ptr = ptr->next;

		double value = Calculate_Wallet_Value(BLOCK_HISTORY_DIRECTORY_PATH, ptr->address, -1);

		B64_Encode(ptr->address, sizeof(_Wallet_Address), &address_base64_buffer);
		printf("%s\t : %.2f%%\t : Value: %.20f\n", address_base64_buffer, ((100.0*value)/coins_in_circulation), value);
		free(address_base64_buffer);
	}

	Wallet_Chain_Recursive_Free(wallet_chain);
}

void Wallet_Calculate_Value_CommandLine() {
	printf("Please enter wallet address (as base64): \n");
	char buffer[256];
	fgets(buffer, sizeof(buffer), stdin);
	byte * address;
	B64_Decode(buffer, &address);

	double value = Calculate_Wallet_Value(BLOCK_HISTORY_DIRECTORY_PATH, address, -1);
	
	printf("This wallet has %.20f GreenCoin.\n", value);
}

void Print_Demo_Keys() {
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

	DSA_Create_Keys(priv_key, pub_key);
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

	DSA_Create_Keys(priv_key, pub_key);
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

int wallet_exit = 0;
int Wallet_Exit() {
	wallet_exit = 1;
	return wallet_exit;
}

int Wallet_Help() {
	printf("Wallet help menu: \n");
	for (int i = 0; i < sizeof(WALLET_COMMANDS) / sizeof(WALLET_COMMANDS[0]); i++) {
		command_t * command = WALLET_COMMANDS[i];
		printf("\t%s", command->command_text);
		int text_len = strlen(command->command_text);
		for (int j = 0; j < 3 - floor((text_len) / 4.0); j++) { printf("\t"); }
		printf("| %s\n", command->command_description);
	}
	return 0;
}

int Wallet_CommandLine_General() {
	printf("Now in wallet command line.\nType 'exit' to return to general command line.\n");
	
	wallet_exit = 0;

	char buffer[1024] = { 0 };

	while (!wallet_exit) {
		printf("> ");
		memset(buffer, 0, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		if (buffer[strlen(buffer) - 1] == '\n') { buffer[strlen(buffer) - 1] = 0x00; }

		for (int i = 0; i < sizeof(WALLET_COMMANDS) / sizeof(WALLET_COMMANDS[0]); i++) {
			if (strcmp(buffer, WALLET_COMMANDS[i]->command_text) == 0) {
				WALLET_COMMANDS[i]->function(NULL, NULL);
			}
		}
	}

	printf("Exiting wallet command line.\n");
}