#include "_Imports.h" 

void checkerr(int Result, const char *ErrorOut)
{
    if(Result < 0)
    {
        perror(ErrorOut);
        exit(EXIT_FAILURE);
    }
}