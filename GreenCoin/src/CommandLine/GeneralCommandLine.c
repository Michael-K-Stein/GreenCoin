
#include "GeneralCommandLine.h"

void Preview_Item(byte * buffer) {
	DSA_Domain_Parameters * params = Get_Domain_Parameters();

	if (memcmp(buffer, GCT_MAGIC, 4) == 0) {
		// GCT file
		Print_Transaction(params, (_Transaction*)(buffer+4));
	}
	else if (memcmp(buffer, GCB_MAGIC, 4) == 0) {
		// GCB file

	}

}