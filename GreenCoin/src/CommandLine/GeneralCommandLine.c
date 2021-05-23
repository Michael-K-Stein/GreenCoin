
#include "GeneralCommandLine.h"


#pragma region Commands

char COMMAND_CREATE_TRANSACTION[64] = "transact\n";
char COMMAND_START_SERVER[64] = "server\n";
//char COMMAND_START_SERVER[64];

#pragma endregion


void Preview_Item(byte * buffer) {
	if (memcmp(buffer, GCT_MAGIC, 4) == 0) {
		// GCT file
		Print_Transaction(stderr, (_Transaction*)(buffer+4));
	}
	else if (memcmp(buffer, GCB_MAGIC, 4) == 0) {
		// GCB file
		Print_Block(stderr, (_Block*)(buffer + 4));
	}

}

int Command_Line() {

	srand(time(NULL));

	// Get ip
	printf("Please enter your ip: ");
	char buf[64] = { 0 };
	fgets(buf, sizeof(buf), stdin);
	unsigned long ip = inet_addr(buf);
	memcpy(LOCALHOST_IP, &ip, sizeof(ip));

	Load_Block_History_Path();

	Load_Domain_Parameters();

	Load_Local_Notary_Signing_Address();

	WSADATA * wsadata = NULL;
	SOCKET * socket = NULL;

	Network_CommandLine_Init(&wsadata, &socket);
	HANDLE handle;


	uint64_t ind = 0;
	while (Block_Index_Exists(ind)) { ind++; }
	FILE* f;
	Open_Block_File(&f, ind - 1);
	if (f != NULL) {
		_Block* b;

		FILE* f;
		Open_Block_File(&f, ind - 1);
		char* block_buffer = (char*)malloc(sizeof(_Block));
		fseek(f, 4, SEEK_SET);
		fread(block_buffer, sizeof(_Block), 1, f);

		char* hash = Hash_SHA256(block_buffer, sizeof(_Block));

		b = Create_Block(ind, hash);

		fclose(f);
		free(hash);

		live_block = b;
	}
	else {
		Create_First_Block(wsadata, socket);
	}

	char buffer[1024] = { 0 };

	while (1) {
		printf("> ");
		memset(buffer, 0, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);

		if (strcmp(buffer, COMMAND_CREATE_TRANSACTION) == 0) {
			Transaction_Demo(wsadata, socket);
		} else if (strcmp(buffer, COMMAND_START_SERVER) == 0) {
			handle = Network_CommandLine_Server(wsadata, socket);
		}
	}
}