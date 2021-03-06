#pragma once

#ifndef __GENERAL_COMMAND_LINE_H
#define __GENERAL_COMMAND_LINE_H

#include "../General/General.h"
#include "../BlockChain/BlockChain.h"
#include "../Network/Network.h"


#pragma region Commands

command_t COMMAND_HELP;
command_t COMMAND_CREATE_TRANSACTION;
command_t COMMAND_START_SERVER;
command_t COMMAND_REQUEST_BLOCKS;
command_t COMMAND_WALLET;
command_t COMMAND_BLOCKCHAIN_LENGTH;
command_t COMMAND_SET_NOTARY;

command_t * COMMANDS[7];

#pragma endregion

char INPUT_BUFFER[1024];
int Query_User_Input();

int Help_Menu();

void Print_Node_List(byte * buffer, long * buffer_size);

void Preview_Item(byte * buffer, long * buffer_size);

int Command_Line();

#endif // !__GENERAL_COMMAND_LINE_H