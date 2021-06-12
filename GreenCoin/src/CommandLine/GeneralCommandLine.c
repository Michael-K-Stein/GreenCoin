
#include "GeneralCommandLine.h"


#pragma region Commands

command_t COMMAND_HELP = {
"help",
"Displays help menu.",
Help_Menu
};
command_t COMMAND_CREATE_TRANSACTION = {
	"transact",
	"Opens transaction command-line where transactions are made and executed in order to transfer funds between wallets.",
	Transaction_Demo
};
command_t COMMAND_START_SERVER = {
	"server",
	"Starts local server which accepts P2P connections from other nodes. Starting a server is a prerequisite to block mining.",
	Network_CommandLine_Server
};
command_t COMMAND_REQUEST_BLOCKS = {
	"gcbh",
	"GreenCoin Block History. Requests blockchain history from all known nodes. Use this to keep up to date with the blockchain.",
	Network_CommandLine_Request_Blocks
};
command_t COMMAND_WALLET = {
	"wallet",
	"Opens wallet command-line (avaliable offline). Use this to monitor wallet funds and create wallets.",
	Wallet_CommandLine_General
};
command_t COMMAND_BLOCKCHAIN_LENGTH = {
	"gcbl",
	"GreenCoin BlockChain Length. Requests the blockchain length from all known nodes.",
	Network_Request_Blockchain_Length_Print
};
command_t COMMAND_SET_NOTARY = {
	"notary",
	"Set the local notary signing address. This address will be used when mining blocks to award you your fee.",
	Load_Local_Notary_Signing_Address
};

command_t * COMMANDS[7] = { &COMMAND_HELP, &COMMAND_CREATE_TRANSACTION, &COMMAND_START_SERVER, &COMMAND_REQUEST_BLOCKS, &COMMAND_BLOCKCHAIN_LENGTH, &COMMAND_WALLET, &COMMAND_SET_NOTARY };


#pragma endregion

int Help_Menu() {
	printf("General help menu: \n");
	for (int i = 0; i < sizeof(COMMANDS) / sizeof(COMMANDS[0]); i++) {
		command_t * command = COMMANDS[i];
		printf("\t%s", command->command_text);
		int text_len = strlen(command->command_text);
		for (int j = 0; j < 3 - floor((text_len) / 4.0); j++) { printf("\t"); }
		printf("| %s\n", command->command_description);
	}
	return 0;
}

void Print_Node_List(byte * buffer, long * buffer_size) {
	/*
		The value of a node is a representation of how often they are online and if they send fraudulent blocks/transactions.
		A node with a high value means it has been unreliable.
		After a certain threshold, a node should no longer be connected to, and should be banned.
		This will essentially remove malicious nodes from the network as after a few nefarious actions, they will be banned from the network indefinetly.
	*/
	long ind = 0;
	unsigned char * ptr = buffer;

	fprintf(stderr, "Known network nodes: \n");

	while (ind < (*buffer_size) - 4) {

		fprintf(stderr, "IP: %hhu.%hhu.%hhu.%hhu \t|\t Value: %hhu\n", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4]);

		ind += 5;
		ptr += 5;
	}
}

void Preview_Item(byte * buffer, long * buffer_size) {
	if (memcmp(buffer, GCT_MAGIC, 4) == 0) {
		// GCT file
		Print_Transaction(stderr, (_Transaction*)(buffer+4));
	}
	else if (memcmp(buffer, GCB_MAGIC, 4) == 0) {
		// GCB file
		Print_Block(stderr, (_Block*)(buffer + 4));
	}
	else if (memcmp(buffer, GCNL_MAGIC, 4) == 0) {
		// GCNL file
		Print_Node_List(buffer + 4, buffer_size);
	}

}

int Query_User_Input() {
	memset(INPUT_BUFFER, 0, sizeof(INPUT_BUFFER));
	char * ptr = fgets(INPUT_BUFFER, sizeof(INPUT_BUFFER), stdin);
	if (ptr == NULL) { return -1; }
	return 0;
}

int Command_Line() {

	srand(time(NULL));

	printf("Would you like to go online? ( Y / N ): ");
	Query_User_Input();
	int is_online = (strcmp(INPUT_BUFFER, "Y\n") == 0) || (strcmp(INPUT_BUFFER, "y\n") == 0) || (strcmp(INPUT_BUFFER, "Yes\n") == 0) || (strcmp(INPUT_BUFFER, "yes\n") == 0);
	printf("Online: "); 
	if (is_online) { printf_Success("TRUE\n"); }
	else { printf_Error("FALSE\n"); }

	WSADATA * wsadata = NULL;
	SOCKET * socket = NULL;
	if (is_online) {
		Network_CommandLine_Init(&wsadata, &socket);
		Network_Print_IP();

		// Get ip
		printf("Please enter your ip: ");
		char buf[64] = { 0 };
		fgets(buf, sizeof(buf), stdin);
		unsigned long ip = inet_addr(buf);
		memcpy(LOCALHOST_IP, &ip, sizeof(ip));

		printf_Info("Initializing server on ip: %hhu.%hhu.%hhu.%hhu\n", LOCALHOST_IP[0], LOCALHOST_IP[1], LOCALHOST_IP[2], LOCALHOST_IP[3]);
		Network_Locate_Nodes(&wsadata, &socket);
	}
	Load_Block_History_Path();

	Load_Domain_Parameters();




	uint64_t ind = 0;
	while (Block_Index_Exists(ind)) { ind++; }
	BlockChainLength = ind;

	HANDLE server_handle;

	char buffer[1024] = { 0 };
	while (1) {
		printf("> ");
		memset(buffer, 0, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		if (buffer[strlen(buffer) - 1] == '\n') { buffer[strlen(buffer) - 1] = 0x00; }

		for (int i = 0; i < sizeof(COMMANDS) / sizeof(COMMANDS[0]); i++) {
			if (strcmp(buffer, COMMANDS[i]->command_text) == 0) {
				COMMANDS[i]->function(wsadata, socket);
			}
		}
	}
}