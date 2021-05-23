
#include "PrettyPrint.h"

void Print_Red() { printf(RED); }
void Print_Green() { printf(GRN); }
void Print_Yellow() { printf(YEL); }
void Print_Reset() { printf(reset); }

void Print_Error() { Print_Red(); }
void Print_Success() { Print_Green(); }
void Print_Info() { Print_Yellow(); }