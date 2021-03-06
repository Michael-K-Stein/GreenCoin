#pragma once

#ifndef __BLOCKCHAIN_H
#define __BLOCKCHAIN_H


#define MAXIMUM_AMOUNT_OF_TRANSACTIONS_ON_LEDGER 64
//const uint16_t MAXIMUM_AMOUNT_OF_TRANSACTIONS_ON_LEDGER = 64;

#include "Transaction.h"
#include "../General/FileIO.h"
#include <time.h>

#define INITIAL_BLOCK_MINING_FEE	300
//const uint16_t INITIAL_BLOCK_MINING_FEE = 300;
#define MINING_FEE_DECAY			0.9999997
//const double MINING_FEE_DECAY = 0.9999997;

uint64_t NETWORK_BLOCK_REQUEST_GLOBAL_PLACEHOLDER;

char BLOCK_HISTORY_DIRECTORY_PATH[256];

uint64_t MIN_STRENGTH;

_Wallet_Address LOCAL_NOTARY_SIGNING_ADDRESS;

typedef struct _TimeStamp {
	uint32_t Unix_Time;
} _TimeStamp;
static char DateTimeBuffer[128];
char * HumanFormatDateTime(_TimeStamp * timestamp);
char * HumanFormatDateTimeInt(uint32_t timestamp);

typedef struct _Hash {
	uint32_t Bytes[8];
} _Hash;
void Print_Hash(FILE * fstream, _Hash * hash);

typedef uint32_t uint256_t[8];
void Print_uint256(FILE * fstream, uint256_t * r);

typedef struct _Block _Block;

struct _Block {

	/*	The index of the block in the chain.
	*	This will be used to decide which chain of blocks is the truth.
	*	The block index will increase by one each block.

	*	If two blocks have the same index, a second chain "of truth" will be created.
	*	Once one chain exeeds the other chain's length by a predifined (or relative) amount, the shorter chain will be invalidated.
	*/
	uint64_t Block_Index;


	/*
	*	Information about the creation time of the block.
	*	This will be the time the first information was written to the block, not the time it was signed.
	*/
	_TimeStamp Time_Stamp;


	/*
	*	The calculated hash of the previous block.
	*	This is used to make sure the hashes are properly interconnected.
	*/
	_Hash Previous_Block_Hash;


	/*
	*	An array of the transactions on the ledger.
	*/
	_Transaction Transactions[MAXIMUM_AMOUNT_OF_TRANSACTIONS_ON_LEDGER];

	/*
	*	The wallet address of the notary who verified and signed the current block.
	*	This address will be awarded all the fees from the transactions on this block.
	*/
	_Wallet_Address Notary_Address;


	/*
	*	256 bit number which is used in order to "control" the resulting hash
	*/
	uint256_t Block_Validation;


	///*
	//*	A linked chain of blocks, as Unary Nodes.
	//*/
	//_Block * Next_Block;
};

_Block * live_block;

uint64_t Calculate_Block_Strength(_Block * block);

_Block * Create_Block(uint64_t new_block_index, char * previous_block_hash_ptr);

error_t Validate_Block(void * wsadata, void * socket, _Block * block, uint64_t desired_strength);

error_t Append_Transaction(void * wsadata, void * socket, _Block * block, _Transaction * transaction);

double Calculate_Block_Total_Miner_Fees(_Block * block);

void Print_Block(FILE * fstream, _Block * block);

int Block_Exists(_Block * block, int block_size);
int Block_Index_Exists(uint64_t ind);
error_t Open_Block_File(FILE ** f, uint64_t block_ind);

error_t Export_Block(FILE * fstream, _Block * block);
error_t Export_To_File(char * dir_path, _Block * block);


error_t Load_Local_Notary_Signing_Address();
error_t Load_Block_History_Path();

error_t Create_First_Block(void * wsadata, void * socket);

error_t Verify_Block(void * wsadata, void * socket, _Block * block, int block_size, int from_history);

//void BlockChain_Demo();

#endif // !__BLOCKCHAIN_H