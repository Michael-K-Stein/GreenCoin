#pragma once

#include "../Wallet/Wallet.h"

#include <stdint.h>

typedef struct _Signature {
	unsigned char Signature[73];
} _Signature;

typedef struct _Transaction {
	// Index of the transaction on the ledger
	uint16_t Index;

	// Address of the sender
	_Wallet_Address Sender;

	// Address of the reciever
	_Wallet_Address Reciever;

	// Value of the transaction in GreenCoins
	double Value;

	// Signature in order to validate sender
	_Signature Signature;
} _Transaction;