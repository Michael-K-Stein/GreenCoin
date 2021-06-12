#pragma once

#ifndef __GENERAL_H
#define __GENERAL_H

#include <string.h>
#include <stdlib.h>
#include "../Cryptography/DSA/DSA.h"

typedef unsigned char byte;

typedef struct Command command_t;
struct Command {
	char command_text[64];
	char command_description[256];
	int(*function)(void *, void *);
};

// Magics
char GCT_MAGIC[4];// = { 'G', 'C', 'T', 0x33 }; // GreenCoin Transaction
char GCB_MAGIC[4];// = { 'G', 'C', 'B', 0x33 }; // GreenCoin Block
char GCNL_MAGIC[4];// = { 'G', 'C', 'N', 'L' }; // GreenCoin Node List

uint64_t BlockChainLength;
uint64_t Get_Blockchain_Length();

void Load_Domain_Parameters();

#endif // !__GENERAL_H
