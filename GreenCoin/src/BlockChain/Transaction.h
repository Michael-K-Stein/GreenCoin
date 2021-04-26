#pragma once

#ifndef __TRANSACTION_H
#define __TRANSACTION_H

#include "../Wallet/Wallet.h"

typedef struct _Signature {
	uint_t r[5];
	uint_t s[5];
} _Signature;

typedef struct _Transaction {
	// Index of the transaction on the ledger
	uint32_t Index;

	// Address of the sender
	_Wallet_Address Sender;

	// Address of the reciever
	_Wallet_Address Reciever;

	// Fees - This can currently be disregarded
	float Fee;

	// Value of the transaction in GreenCoins
	double Value;

	// Signature in order to validate sender
	_Signature Signature;
} _Transaction;
void Print_Transaction_Signature_Part(uint_t * q);
void Print_Transaction_Wallet_Address(uint_t * q);
void Print_Transaction(DSA_Domain_Parameters * params, _Transaction * transaction);

void Transaction_Export(FILE * fstream, _Transaction * transaction);
void Transaction_Export_To_File(char * file_path, _Transaction * transaction);

void Sign_Transaction(DSA_Domain_Parameters * params, _Transaction * transaction, BN * priv_key);

void Transaction_Demo();

#endif // !__TRANSACTION_H