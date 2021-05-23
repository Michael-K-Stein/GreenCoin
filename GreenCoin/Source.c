
#include "src/BlockChain/BlockChain.h"
#include "src/CommandLine/ExternalCommandLine.h"
#include "src/Network/Network.h"

#include <stdio.h>

#include "src/General/Print/PrettyPrint.h"

int Demo() {

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

	HANDLE server_thread = Network_Demo(wsadata, socket);

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

	while (1) {
		Transaction_Demo_Spam(wsadata, socket);
	}
}

int main_c(int argc, char ** argv) {

	if (argc > 1) { Execute_External_Commands(argc, argv); }
	
	
	
	else {
		Command_Line();
		//Demo();
	}
	
	if (argc > 1) {
		printf("Press ENTER to close.\n");
		getchar();
	}
}