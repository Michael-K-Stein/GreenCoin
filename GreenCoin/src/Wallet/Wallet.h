#pragma once

#ifndef __WALLET_H
#define __WALLET_H


#include "../Cryptography/DSA/DSA.h"
#include "../General/Base64/Base64.h"
#include "../Cryptography/Hash/SHA256.h"

command_t COMMAND_GENERATE_WALLET;
command_t COMMAND_EXIT;
command_t COMMAND_WALLET_VALUE;

typedef uint_t _Wallet_Address[32];


double Calculate_Wallet_Value(char * dir_path, _Wallet_Address pk, uint64_t up_to_block_index);

int Is_Wallet_Value_Greater(char * dir_path, _Wallet_Address pk, uint64_t up_to_block_index, double thresh);

int Wallet_Chain_Contains_Address(struct Wallet_Piece * wallet_chain, _Wallet_Address wallet_address);
int Wallet_Add_Address_To_Chain(struct Wallet_Piece * wallet_chain, _Wallet_Address wallet_address);
int Wallet_Chain_Recursive_Free(struct Wallet_Piece * wallet_chain);
void Wallet_Calculate_Values();

void Wallet_Calculate_Value_CommandLine();

void Print_Demo_Keys();

DSA_Public_Key * Get_Random_Public_Key();

int wallet_exit;
int Wallet_Exit();

int Wallet_Help();

int Wallet_CommandLine_General();

#endif // !__WALLET_H