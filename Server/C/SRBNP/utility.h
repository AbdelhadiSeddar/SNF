#ifndef UTILITY_H
#define UTILITY_H

extern void checkerr(int Result, const char *ErrorOut);

extern char *Fbyte_FROM_str(const char *String);
extern char *Fbyte_FROM_int(int Size);
extern int Fbyte_TO_int(const char *Fbyte);
extern int setnonblocking(int _sock);
#endif