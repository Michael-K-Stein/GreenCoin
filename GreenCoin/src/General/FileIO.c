
#include "FileIO.h"

int File_Valid(char * file_path) {
	FILE * f;
	fopen_s(&f, file_path, "r");
	int valid = f != NULL;
	fclose(f);
	return valid;
}
long Get_File_Size(char * file_path) {
	FILE * f;
	fopen_s(&f, file_path, "r");
	if (f == NULL) { return -1; }
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fclose(f);
	return size;
}
long Load_File(char * file_path, void ** buffer) {

	long file_size = Get_File_Size(file_path);
	if (file_size < 0) { return file_size; }

	FILE * f;
	fopen_s(&f, file_path, "r");

	*buffer = malloc(file_size);

	if (*buffer == NULL) { fclose(f); return -1; }

	long read = fread(*buffer, sizeof(char), file_size, f);

	if (read != file_size) { fclose(f); return -1; }

	return file_size;
}