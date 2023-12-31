#include "_Imports.h" 
#include "utility.h"

void checkerr(int Result, const char *ErrorOut)
{
    if(Result < 0)
    {
        perror(ErrorOut);
        exit(EXIT_FAILURE);
    }
}


char* Fbyte_FROM_str(const char* String)
{
    int Strlen = strlen(String);
    return Fbyte_FROM_int(Strlen);
}
char *Fbyte_FROM_int(int Size) 
{
    char *re = malloc(5*sizeof(char));
    for(int i = 0 ; i < 4 ; i++)
    {
        re[3 - i] = Size % 10 + 48;
        Size /= 10;
    }
    re[4] = '\0';
    if(Size > 0)
        return "0000\0";
    return re;
}
int Fbyte_TO_int(const char Fbyte[5])
{
    int re =0;
    for(int i = 0 ; i < 4 ; i++)
    {
        if(Fbyte[3 - i] < 48 || Fbyte[3 - i] > 57)
            return -1;

        re = Fbyte[3 - i] - 48;
        re *= 10;
    }
    return re;
}
int setnonblocking(int _sock)
{
    int result;
    int flags;

    flags = fcntl(_sock, F_GETFL, 0);

    if (flags == -1)
    {
        return -1;  // error
    }

    flags |= O_NONBLOCK;

    result = fcntl(_sock , F_SETFL , flags);
    return result;
}