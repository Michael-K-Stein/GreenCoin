
#include "PrettyPrint.h"

void Print_Red() { printf(RED); }
void Print_Green() { printf(GRN); }
void Print_Yellow() { printf(YEL); }
void Print_Reset() { printf(reset); }

void Print_Error() { Print_Red(); }
void Print_Success() { Print_Green(); }
void Print_Info() { Print_Yellow(); }


#ifdef _WIN32
// Some old MinGW/CYGWIN distributions don't define this:
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
#endif

static HANDLE stdoutHandle;
static DWORD outModeInit;

void setupConsole(void) {
	 	DWORD outMode = 0;
	 	stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		 	if (stdoutHandle == INVALID_HANDLE_VALUE) {
		 		exit(GetLastError());
		
	}
	
		 	if (!GetConsoleMode(stdoutHandle, &outMode)) {
		 		exit(GetLastError());
	
	}
	
		 	outModeInit = outMode;
	
		     // Enable ANSI escape codes
		 	outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	
		 	if (!SetConsoleMode(stdoutHandle, outMode)) {
		 		exit(GetLastError());
		
	}
	
}

void restoreConsole(void) {
	     // Reset colors
		     printf("\x1b[0m");
	
	     // Reset console mode
		 	if (!SetConsoleMode(stdoutHandle, outModeInit)) {
		 		exit(GetLastError());
		
	}
	
}
#else
void setupConsole(void) {}

void restoreConsole(void) {
	     // Reset colors
		     printf("\x1b[0m");
	
}
#endif