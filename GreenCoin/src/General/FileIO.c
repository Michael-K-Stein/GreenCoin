
#include "FileIO.h"

int File_Valid(char * file_path) {
	FILE * f;
	fopen_s(&f, file_path, "rb");
	int valid = f != NULL;
	fclose(f);
	return valid;
}
long Get_File_Size(char * file_path) {
	FILE * f;
	fopen_s(&f, file_path, "rb");
	if (f == NULL) { return -1; }
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fclose(f);
	return size;
}
long Load_File(char * file_path, void ** buffer) {

	//long file_size = Get_File_Size(file_path);
	//if (file_size < 0) { return file_size; }

	FILE * f;
	fopen_s(&f, file_path, "rb");
	if (f == NULL) { return -1; }

	fseek(f, 0, SEEK_END);
	long file_size = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (file_size < 0 && file_size != -1) { fclose(f); return file_size; }

	*buffer = malloc(file_size);

	if (*buffer == NULL) { fclose(f); return -1; }

	long read = fread(*buffer, sizeof(char), file_size, f);

	if (read != file_size) { fclose(f); free(*buffer); return -1; }

	fclose(f);

	return file_size;
}