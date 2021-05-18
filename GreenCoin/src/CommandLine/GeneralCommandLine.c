
#include "GeneralCommandLine.h"

void Preview_Item(byte * buffer) {
	if (memcmp(buffer, GCT_MAGIC, 4) == 0) {
		// GCT file
		Print_Transaction(stderr, params, (_Transaction*)(buffer+4));
	}
	else if (memcmp(buffer, GCB_MAGIC, 4) == 0) {
		// GCB file
		Print_Block(stderr, params, (_Block*)(buffer + 4));
	}

}