#pragma once

#define MAXIMUM_AMOUNT_OF_TRANSACTIONS_ON_LEDGER 1024

#include <stdint.h>

#include "Transaction.h"

typedef struct _TimeStamp {
	uint32_t Unix_Time;
} _TimeStamp;

typedef struct _Hash {
	unsigned char Bytes[32];
} _Hash;

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
	*	A linked chain of blocks, as Unary Nodes.
	*/
	_Block * Next_Block;
};