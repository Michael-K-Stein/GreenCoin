#pragma once

#ifndef __PrettyPrint
#define __PrettyPrint

#include "ANSI_Color_Codes.h"

#ifndef printf_Error
#define printf_Error(...)		Print_Error();fprintf(stderr, __VA_ARGS__);Print_Reset();
#endif
#ifndef printf_Success
#define printf_Success(...)		Print_Success();fprintf(stderr, __VA_ARGS__);Print_Reset();
#endif
#ifndef printf_Info
#define printf_Info(...)		Print_Info();fprintf(stderr, __VA_ARGS__);Print_Reset();
#endif

void Print_Red();
void Print_Green();
void Print_Yellow();
void Print_Reset();

void Print_Error();
void Print_Success();
void Print_Info();

#endif // !__PrettyPrint
