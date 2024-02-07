#ifndef UTILITY_H
#define UTILITY_H

#include <SRBNP/SRBNP.h>
extern void checkerr(int Result, const char *ErrorOut);

extern char *srbnp_Fbyte_FROM_str(const char *String);
extern char *srbnp_Fbyte_FROM_int(int Size);
extern int srbnp_Fbyte_TO_int(const char *Fbyte);
extern int srbnp_setnonblocking(int _sock);
#endif