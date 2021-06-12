
#include "ExternalCommandLine.h"

int Execute_External_Commands(int argc, char ** argv) {
	printf("'%s'\n", argv[0]);
	
	Load_Domain_Parameters();

	if (argc == 2) {
		if (File_Valid(argv[1])) {
			// Open file with GreenCoin.exe
			char * file_path = argv[1];

			byte * buffer;
			long buffer_size = Load_File(file_path, &buffer);
			if (buffer_size > 0) {
				Preview_Item(buffer, &buffer_size);
			}
			else {
				printf("Could not load or read file!\n");
			}
		}
	}
}