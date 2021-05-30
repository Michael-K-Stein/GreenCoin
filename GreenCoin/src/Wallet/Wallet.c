
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
command_t WALLET_COMMAND_EXIT = {
	"exit",
	"Exits the wallet command-line.",
	Wallet_Exit
};

command_t * WALLET_COMMANDS[4] = { &WALLET_COMMAND_HELP, &WALLET_COMMAND_GENERATE_WALLET, &WALLET_COMMAND_WALLET_VALUE, &WALLET_COMMAND_EXIT };


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