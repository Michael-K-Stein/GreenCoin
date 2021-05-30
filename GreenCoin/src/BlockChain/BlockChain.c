
#include "BlockChain.h"

#include "../Wallet/Wallet.h"
#include "../General/Print/PrettyPrint.h"

char BLOCK_HISTORY_DIRECTORY_PATH[256] = "";

char * HumanFormatDateTime(_TimeStamp * timestamp) {
	time_t rawtime = timestamp->Unix_Time;
	struct tm  ts;
	// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
	localtime_s(&ts, &rawtime);
	strftime(DateTimeBuffer, sizeof(DateTimeBuffer), "%Y-%m-%d %H:%M:%S", &ts);
	return DateTimeBuffer;
}
char * HumanFormatDateTimeInt(uint32_t timestamp) {
	time_t rawtime = timestamp;
	struct tm  ts;
	// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
	localtime_s(&ts, &rawtime);
	strftime(DateTimeBuffer, sizeof(DateTimeBuffer), "%Y-%m-%d %H:%M:%S", &ts);
	return DateTimeBuffer;
}

void Print_Hash(FILE * fstream, _Hash * hash) {
	fprintf(fstream, "  ");
	for (int i = 7; i >= 0; i--) {
		fprintf(fstream, "%.8X ", hash->Bytes[i]);
		if (i % 4 == 0) { fprintf(fstream, "\n  "); }
	}
	fprintf(fstream, "\n");
}

void Print_uint256(FILE * fstream, uint256_t * r) {
	fprintf(fstream, "  ");
	for (int i = 7; i >= 0; i--) {
		fprintf(fstream, "%.8X ", (*r)[i]);
		if (i % 4 == 0) { fprintf(fstream, "\n  "); }
	}
	fprintf(fstream, "\n");
}

uint64_t Calculate_Block_Strength(_Block * block) {
	char * digest = Hash_SHA256(block, sizeof(_Block));

	int i = 0;
	while (digest[i] == '0') { i++; }
	
	free(digest);

	return (uint64_t)(pow(2, i) - 1);
}

_Block * Create_Block(uint64_t new_block_index, char * previous_block_hash_ptr) {
	_Block * block = (_Block*)calloc(1, sizeof(_Block));

	block->Time_Stamp.Unix_Time = time(NULL);

	block->Block_Index = new_block_index;

	BN hs; BN_Init_Stack(&hs);
	BN_Import_Hex_String(&hs, previous_block_hash_ptr, 64, BN_BIG_ENDIAN);
	memcpy(block->Previous_Block_Hash.Bytes, hs.data, 8 * sizeof(uint32_t));

	free(hs.data);

	memset(block->Block_Validation, 0, sizeof(uint256_t));

	return block;
}

error_t Validate_Block(void * wsadata, void * socket, _Block * block, uint64_t desired_strength) {

	//memcpy(block->Notary_Address, y->data, y->size * BN_INT_SIZE);
	memcpy(block->Notary_Address, LOCAL_NOTARY_SIGNING_ADDRESS, sizeof(LOCAL_NOTARY_SIGNING_ADDRESS));

	BN c;
	BN_Init_Stack(&c);
	BN_Resize(&c, 32 / BN_INT_SIZE);
	memcpy(c.data, block->Block_Validation, sizeof(uint256_t));

	uint64_t strength = Calculate_Block_Strength(block);
	//printf("Block strength: %lu\n", strength);

	while (strength < desired_strength) {
		BN_Add_Int(&c, &c, 1);
		memcpy(block->Block_Validation, c.data, sizeof(uint256_t));
		strength = Calculate_Block_Strength(block);
		//printf("Block strength: %u\n", strength);
	}

	free(c.data);

	printf_Success("Block has been signed with strength: %lu!\n", strength);

	Export_To_File(BLOCK_HISTORY_DIRECTORY_PATH, block);

	Network_Broadcast_Block(wsadata, socket, block, sizeof(_Block));

	return ERROR_NONE;
}

error_t Append_Transaction(void * wsadata, void * socket, _Block * block, _Transaction * transaction) {
	//uint32_t transaction_index = transaction->Index;

	//_Transaction * target = &(block->Transactions[transaction_index]);
	_Transaction * target = &(block->Transactions[0]);

	_Transaction zero_transaction; memset(&zero_transaction, 0, sizeof(_Transaction));

	int index = 0;
	while (memcmp(&(block->Transactions[index]), &zero_transaction, sizeof(_Transaction)) != 0) {
		index++;
	}

	target = &(block->Transactions[index]);

	// Make sure that there aren't two duplicate transactions on a block.
	// This protects the system from a fraud duplicating a valid transaction.
	for (int i = 0; i < index; i++) {
		if (memcmp(transaction, &(block->Transactions[i]), sizeof(_Transaction)) == 0) { return ERROR_BLOCK_TRANSACTION_DUPLICATE; }
	}

	if (index >= MAXIMUM_AMOUNT_OF_TRANSACTIONS_ON_LEDGER) { return ERROR_BLOCK_TRANSACTION_SLOT_IN_USE; }

	//if (memcmp(target, &zero_transaction, sizeof(_Transaction)) != 0) { return ERROR_BLOCK_TRANSACTION_SLOT_IN_USE; }

	if (Verify_Transaction(transaction) != SIGNATURE_VALID) { return ERROR_BLOCK_TRANSACTION_SIGNATURE_INVALID; }

	double value = Calculate_Wallet_Value(BLOCK_HISTORY_DIRECTORY_PATH, transaction->Sender, block->Block_Index);

	if (value < transaction->Value + transaction->Fee) { return ERROR_BLOCK_TRANSACTION_INSUFFICIENT_FUNDS; }

	memcpy(target, transaction, sizeof(_Transaction));

	printf_Success("A valid transaction has been received and processed.\n");

	if (index == MAXIMUM_AMOUNT_OF_TRANSACTIONS_ON_LEDGER - 1) {
		// Block is full. You may now sign it.
		Validate_Block(wsadata, socket, live_block, 7);

		char * hash = Hash_SHA256(live_block, sizeof(_Block));
		live_block = Create_Block(live_block->Block_Index + 1, hash);
		free(hash);
	}

	return ERROR_BLOCK_TRANSACTION_NONE;
}

double Calculate_Block_Total_Miner_Fees(_Block * block) {
	double total = 0;
	double mining_fee = (double)INITIAL_BLOCK_MINING_FEE * pow((double)MINING_FEE_DECAY, block->Block_Index);
	total += mining_fee;
	
	for (int i = 0; i < MAXIMUM_AMOUNT_OF_TRANSACTIONS_ON_LEDGER; i++) {
		total += block->Transactions[i].Fee;
	}

	return total;
}

void Print_Block(FILE * fstream, _Block * block) {
	fprintf(fstream, "=== === === Block #%lu === === ===\n", block->Block_Index);
	fprintf(fstream, "\tCreation Time: %s\n", HumanFormatDateTime(&(block->Time_Stamp)));

	if (block->Block_Index != 0) {
		fprintf(fstream, "\t--- Begin Transactions ---\n");
		for (int i = 0; i < MAXIMUM_AMOUNT_OF_TRANSACTIONS_ON_LEDGER; i++) {
			Print_Transaction(fstream, &(block->Transactions[i]));
		}
		fprintf(fstream, "\t--- End Transactions ---\n");
	}
	else {
		fprintf(fstream, "%s", block->Transactions);
	}

	fprintf(fstream, "\tPrevious block hash: \n");
	Print_Hash(fstream, &(block->Previous_Block_Hash));

	fprintf(fstream, "\tNotary Address: \n");
	Print_Transaction_Wallet_Address(fstream, block->Notary_Address);

	fprintf(fstream, "\tVerification: \n");
	Print_uint256(fstream, (block->Block_Validation));

	fprintf(fstream, "\n=== === === End Block #%lu === === ===\n", block->Block_Index);
}

/*
void BlockChain_Demo() {
	char * prev_block_hash = Hash_SHA256("", 0);
	char prev_block_hash_data[64];// = Hash_SHA256("", 0);

	printf("Please enter your public key (as base64): \n");
	char key_64[180]; fgets(key_64, 180, stdin);

	byte * key;
	B64_Decode(key_64, &key);

	DSA_Public_Key * pub_key=0; DSA_Init_Public_Key(&pub_key);
	BN_Resize(pub_key, 32);
	memcpy(pub_key->data, key, sizeof(_Wallet_Address));
	free(key);

	
	char file_path[256];
	char * buffer;
	_Block * block;
	_Transaction transaction_tmp;
	size_t length = 0;

	uint64_t c = 303;
	while (1) {
		block = Create_Block(c++, prev_block_hash);
		free(prev_block_hash);

		for (uint32_t i = 0; i < MAXIMUM_AMOUNT_OF_TRANSACTIONS_ON_LEDGER; i++) {
			sprintf_s(file_path, 256, "C:\\Users\\stein\\Desktop\\GreenCoin\\Globals\\Demo_Transactions\\Test_%u.GCT", i);
			
			length = Load_File(file_path, &buffer);
			if (length != -1) {
				memcpy(&transaction_tmp, buffer + 4, sizeof(_Transaction));

				Append_Transaction(block, &transaction_tmp);

				free(buffer);
			}

		}
		
		Validate_Block(block, 1);
		
		Export_To_File("C:\\Users\\stein\\Desktop\\GreenCoin\\Demo\\Blocks", block);

		//free(prev_block_hash);
		prev_block_hash = Hash_SHA256(block, sizeof(_Block));
		//memcpy(prev_block_hash_data, prev_block_hash, 64);
		free(block);
	}

	//Print_BlockChain(stderr, block, params);
}
*/

error_t Export_Block(FILE * fstream, _Block * block) {
	size_t count = fwrite(block, sizeof(_Block), 1, fstream);
	if (count == 1) {
		return ERROR_NONE;
	}
	else {
		return ERROR_WRITE_TO_FILE;
	}
}
error_t Export_To_File(char * dir_path, _Block * block) {
	FILE * f;
	char buffer[256] = { 0 };
	sprintf_s(buffer, 256, "%s\\%lu.GCB", dir_path, block->Block_Index);
	fopen_s(&f, buffer, "wb");
	if (f == NULL) { return ERROR_OPEN_FILE; }
	error_t error = ERROR_NONE;
	error |= !(fwrite(GCB_MAGIC, sizeof(char), 4, f) == 4);
	error |= Export_Block(f, block);
	fclose(f);
	return error;
}

error_t Load_Local_Notary_Signing_Address() {
	FILE* f;
	errno_t err = fopen_s(&f, "Local_Notary_Address.GCWA", "rb");
	if (err == 0) {
		fread_s(LOCAL_NOTARY_SIGNING_ADDRESS, sizeof(LOCAL_NOTARY_SIGNING_ADDRESS), sizeof(LOCAL_NOTARY_SIGNING_ADDRESS), 1, f);
	}
	else {
		err = fopen_s(&f, "Local_Notary_Address.GCWA", "wb");
		printf("Please enter the address to be used when mining blocks (as base64): \n");
		char buf[1024];
		fgets(buf, 1024, stdin);
		byte* out;
		B64_Decode(buf, &out);
		memcpy(LOCAL_NOTARY_SIGNING_ADDRESS, out, sizeof(LOCAL_NOTARY_SIGNING_ADDRESS));
		fwrite(LOCAL_NOTARY_SIGNING_ADDRESS, sizeof(LOCAL_NOTARY_SIGNING_ADDRESS), 1, f);
		free(out);
	}
	fclose(f);
	printf("Mining address is: \n");
	Print_Transaction_Wallet_Address(stdout, LOCAL_NOTARY_SIGNING_ADDRESS);
}

error_t Load_Block_History_Path() {
	FILE * f;
	errno_t err = fopen_s(&f, "Block_History_Path.GCTXT", "r");
	if (err == 0) {
		fread_s(BLOCK_HISTORY_DIRECTORY_PATH, 256, sizeof(char), 256, f);
	}
	else {
		err = fopen_s(&f, "Block_History_Path.GCTXT", "w");
		printf("Please enter directory path for Block History: ");
		fgets(BLOCK_HISTORY_DIRECTORY_PATH, 256, stdin);
		fwrite(BLOCK_HISTORY_DIRECTORY_PATH, sizeof(char), 256, f);
	}
	fclose(f);

	printf("Blockchain is saved to: %s\\\n", BLOCK_HISTORY_DIRECTORY_PATH);
	return ERROR_NONE;
}

error_t Create_First_Block(void * wsadata, void * socket) {
	char * null = NULL;
	char * hash = Hash_SHA256(null, 0);
	_Block * b = Create_Block(0, hash);
	free(hash);

	b->Time_Stamp.Unix_Time = time(NULL);


	FILE * f;
	fopen_s(&f, "C:\\Users\\stein\\Desktop\\GreenCoin\\Globals\\INIT_MESSAGE.txt", "r");
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);
	char * Initial_Block_Message = (char*)calloc(size+1, sizeof(char));
	fread(Initial_Block_Message, sizeof(char), size, f);
	fclose(f);

	memcpy(b->Transactions, Initial_Block_Message, size + 1);
	free(Initial_Block_Message);

	Validate_Block(wsadata, socket, b, 7);

	Export_To_File(BLOCK_HISTORY_DIRECTORY_PATH, b);

	free(b);

	return ERROR_NONE;
}

error_t Open_Block_File(FILE ** f, uint64_t block_ind) {
	char buffer[256] = { 0 };
	sprintf_s(buffer, 256, "%s\\%lu.GCB", BLOCK_HISTORY_DIRECTORY_PATH, block_ind);

	fopen_s(f, buffer, "rb");
	if (*f == NULL) { return ERROR_FAILED; }

	return ERROR_NONE;
}

int Block_Index_Exists(uint64_t ind) {
	char buffer[256] = { 0 };
	sprintf_s(buffer, 256, "%s\\%lu.GCB", BLOCK_HISTORY_DIRECTORY_PATH, ind);

	FILE * f;
	fopen_s(&f, buffer, "rb");
	if (f == NULL) { return 0; }
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	if (size != sizeof(_Block)+4) { /* A different block with the same index exists */ return 2; }

	fclose(f);

	return 1;
}
int Block_Exists(_Block * block, int block_size) {
	char buffer[256] = { 0 };
	sprintf_s(buffer, 256, "%s\\%lu.GCB", BLOCK_HISTORY_DIRECTORY_PATH, block->Block_Index);

	FILE * f;
	errno_t err =fopen_s(&f, buffer, "rb");
	if (f == NULL || err != 0) { return 0; }
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	if (size != block_size+sizeof(GCB_MAGIC)) { /* A different block with the same index exists */ return 2; }

	char * buf = (char*)malloc(size);
	fread(buf, 1, size, f);
	fclose(f);

	int is_same = memcmp(buf + 4, block, size - 4) == 0;

	if (is_same) { return 1; }
	else {
		/* A different block with the same index exists */
		return 2;
	}

	return -1;
}

uint64_t MIN_STRENGTH = 7;
error_t Verify_Block(void * wsadata, void * socket, _Block * block, int block_size) {

	// 0 -> No block by this name.
	// 1 -> This exact block exists => duplicate.
	// 2 -> Inconsistant blocks. Two different blockchains exist.
	int block_exists = Block_Exists(block, block_size);

	if (block_exists == 1) { return ERROR_NONE; }

	// Check that the block produces the desired hash
	uint64_t strength = Calculate_Block_Strength(block);
	if (strength < MIN_STRENGTH) {
		return ERROR_BLOCK_WEAK;
	}

	error_t error = ERROR_NONE;
	if (block->Block_Index > 0) {
		// Verify each transaction
		for (int index = 0; index < MAXIMUM_AMOUNT_OF_TRANSACTIONS_ON_LEDGER; index++) {
			_Transaction * transaction = &(block->Transactions[index]);

			if (Verify_Transaction(transaction) != SIGNATURE_VALID) { error |= ERROR_BLOCK_TRANSACTION_SIGNATURE_INVALID; }

			double value = Calculate_Wallet_Value(BLOCK_HISTORY_DIRECTORY_PATH, transaction->Sender, block->Block_Index - 1);

			if (value < transaction->Value + transaction->Fee) { error |= ERROR_BLOCK_TRANSACTION_INSUFFICIENT_FUNDS; }
		}
	}

	if (block->Block_Index > 0) {
		FILE* f;
		Open_Block_File(&f, block->Block_Index - 1);
		if (f == NULL) { return ERROR_FAILED; }
		fseek(f, 4, SEEK_SET);
		_Block b; fread(&b, sizeof(b), 1, f);
		fclose(f);
		char* prev_hash = Hash_SHA256(&b, sizeof(b));

		BN hs; BN_Init_Stack(&hs);
		BN_Import_Hex_String(&hs, prev_hash, 64, BN_BIG_ENDIAN);
		int prev_hash_valid = (memcmp(hs.data, &(block->Previous_Block_Hash), sizeof(_Hash))==0);

		free(hs.data);

		if (prev_hash_valid != 1) { error |= ERROR_FAILED; }
		free(prev_hash);
	}

	if (error == ERROR_NONE) {
		BlockChainLength = block->Block_Index+1;

		Export_To_File(BLOCK_HISTORY_DIRECTORY_PATH, block);

		Network_Broadcast_Block(wsadata, socket, block, block_size);
	}
}