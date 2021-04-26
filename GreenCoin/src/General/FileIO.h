#pragma once

#ifndef __FILE_IO_H
#define __FILE_IO_H

#include <stdio.h>

int File_Valid(char * file_path);
int Get_File_Size(char * file_path);
int Load_File(char * file_path, void ** buffer); // Allocates the neccessary amount of memory to the buffer. Returns the size of the loaded file - size of the buffer.


#endif // !__FILE_IO_H