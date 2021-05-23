#pragma once

#ifndef __GENERAL_COMMAND_LINE_H
#define __GENERAL_COMMAND_LINE_H

#include "../General/General.h"
#include "../BlockChain/BlockChain.h"
#include "../Network/Network.h"


#pragma region Commands

char COMMAND_CREATE_TRANSACTION[64];
char COMMAND_START_SERVER[64];
//char COMMAND_START_SERVER[64];

#pragma endregion




void Preview_Item(byte * buffer);

int Command_Line();

#endif // !__GENERAL_COMMAND_LINE_H