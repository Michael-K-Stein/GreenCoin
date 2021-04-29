
#include "BlockChain.h"

#include "../Wallet/Wallet.h"

char * HumanFormatDateTime(_TimeStamp * timestamp) {
	time_t rawtime = timestamp->Unix_Time;
	struct tm  ts;
	// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
	localtime_s(&ts, &rawtime);
	strftime(DateTimeBuffer, sizeof(DateTimeBuffer), "%Y-%m-%d %H:%M:%S\n", &ts);
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

error_t Validate_Block(_Block * block, BN * y, uint64_t desired_strength) {

	memcpy(block->Notary_Address, y->data, y->size * BN_INT_SIZE);

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

	printf("Block has been signed with strength: %lu!\n", strength);
	return ERROR_NONE;
}

error_t Append_Transaction(_Block * block, _Transaction * transaction, DSA_Domain_Parameters * params) {
	uint32_t transaction_index = transaction->Index;

	_Transaction * target = &(block->Transactions[transaction_index]);

	_Transaction zero_transaction; memset(&zero_transaction, 0, sizeof(_Transaction));

	if (memcmp(target, &zero_transaction, sizeof(_Transaction)) != 0) { return ERROR_BLOCK_TRANSACTION_SLOT_IN_USE; }

	if (Verify_Transaction(params, transaction) != SIGNATURE_VALID) { return ERROR_BLOCK_TRANSACTION_SIGNATURE_INVALID; }

	double value = Calculate_Wallet_Value("C:\\Users\\stein\\Desktop\\GreenCoin\\Globals\\Demo_Transactions", transaction->Sender, block->Block_Index - 1);

	if (value < target->Value + target->Fee) { return ERROR_BLOCK_TRANSACTION_INSUFFICIENT_FUNDS; }

	memcpy(target, transaction, sizeof(_Transaction));

	return ERROR_BLOCK_TRANSACTION_NONE;
}

double Calculate_Block_Total_Miner_Fees(_Block * block) {
	double total = 0;
	double mining_fee = (double)INITIAL_BLOCK_MINING_FEE * pow((double)MINING_FEE_DECAY, block->Block_Index - 1);
	total += mining_fee;
	
	for (int i = 0; i < MAXIMUM_AMOUNT_OF_TRANSACTIONS_ON_LEDGER; i++) {
		total += block->Transactions[i].Fee;
	}

	return total;
}

void Print_Block(FILE * fstream, DSA_Domain_Parameters * params, _Block * block) {
	fprintf(fstream, "=== === === Block #%lu === === ===\n", block->Block_Index);
	fprintf(fstream, "\tCreation Time: %s", HumanFormatDateTime(&(block->Time_Stamp)));

	fprintf(fstream, "\t--- Begin Transactions ---\n");
	for (int i = 0; i < MAXIMUM_AMOUNT_OF_TRANSACTIONS_ON_LEDGER; i++) {
		Print_Transaction(fstream, params, &(block->Transactions[i]));
	}
	fprintf(fstream, "\t--- End Transactions ---\n");

	fprintf(fstream, "\tPrevious block hash: \n");
	Print_Hash(fstream, &(block->Previous_Block_Hash));

	fprintf(fstream, "\tNotary Address: \n");
	Print_Transaction_Wallet_Address(fstream, block->Notary_Address);

	fprintf(fstream, "\tVerification: \n");
	Print_uint256(fstream, (block->Block_Validation));

	fprintf(fstream, "\n=== === === End Block #%lu === === ===\n", block->Block_Index);
}


void BlockChain_Demo() {
	DSA_Domain_Parameters * params = Get_Domain_Parameters();

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

				Append_Transaction(block, &transaction_tmp, params);

				free(buffer);
			}

		}
		
		Validate_Block(block, pub_key, 1);
		
		Export_To_File("C:\\Users\\stein\\Desktop\\GreenCoin\\Demo\\Blocks", block);

		//free(prev_block_hash);
		prev_block_hash = Hash_SHA256(block, sizeof(_Block));
		//memcpy(prev_block_hash_data, prev_block_hash, 64);
		free(block);
	}

	//Print_BlockChain(stderr, block, params);
}

error_t Export_Block(FILE * fstream, _Block * block) {
	size_t count = fwrite(block, sizeof(_Block), 1, fstream);
	if (count == sizeof(_Block)) {
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
	error |= (fwrite(GCB_MAGIC, sizeof(char), 4, f) == 4);
	error |= Export_Block(f, block);
	fclose(f);
	return error;
}