#pragma once

#ifndef __GENERAL_H
#define __GENERAL_H

#include <string.h>
#include <stdlib.h>
#include "../Cryptography/DSA/DSA.h"

typedef unsigned char byte;

// Magics
char GCT_MAGIC[4];// = { 'G', 'C', 'T', 0x33 }; // GreenCoin Transaction
char GCB_MAGIC[4];// = { 'G', 'C', 'B', 0x33 }; // GreenCoin Block




void * Get_Domain_Parameters();

#endif // !__GENERAL_H
