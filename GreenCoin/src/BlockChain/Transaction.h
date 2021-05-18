#pragma once

#ifndef __TRANSACTION_H
#define __TRANSACTION_H

#include "../Wallet/Wallet.h"
//#include "../Network/Network.h"

typedef struct _Signature {
	uint_t r[5];
	uint_t s[5];
} _Signature;

typedef struct _Transaction {
	// Index of the block in the chain
	uint64_t Block_Index;

	/* DEPRACATED
	// Index of the transaction on the ledger
	uint32_t Index;
	*/

	// The time this transaction was signed.
	uint32_t Time;

	/*
		The combination of the block_index and the index are completely unique in the chain,
		 and therefore protect the signature from being compied either again on the same block
		 or at the same position on a different block.
	*/

	// Address of the sender
	_Wallet_Address Sender;

	// Address of the reciever
	_Wallet_Address Reciever;

	// Fees - This can currently be disregarded
	double Fee;

	// Value of the transaction in GreenCoins
	double Value;

	// Signature in order to validate sender
	_Signature Signature;
} _Transaction;
void Print_Transaction_Signature_Part(FILE * fstream, uint_t * q);
void Print_Transaction_Wallet_Address(FILE * fstream, uint_t * q);
void Print_Transaction(FILE * fstream, _Transaction * transaction);

void Transaction_Export(FILE * fstream, _Transaction * transaction);
void Transaction_Export_To_File(char * file_path, _Transaction * transaction);

void Sign_Transaction(_Transaction * transaction, BN * priv_key);

// Returns the value of the transaction with regards to a wallet.
// If the wallet was the sender, deduce the TOTAL value of the transaction.
// If the wallet was the reciever, add the value of the transaction.
double Calculate_Transaction_Change_To_Wallet(_Transaction * transaction, _Wallet_Address pk);

SIGNATURE_VALID_STATE Verify_Transaction(_Transaction * transaction);

void Transaction_Demo(void * wsadata, void * socket);

#endif // !__TRANSACTION_H