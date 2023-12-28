#include "_Imports.h" 

void checkerr(int Result, const char *ErrorOut)
{
    if(Result < 0)
    {
        perror(ErrorOut);
        exit(EXIT_FAILURE);
    }
}


char* fb_strlen(const char* String)
{
    char *re = malloc(5*sizeof(char));
    int Strlen = strlen(String);
    for(int i = 0 ; i < 4 ; i++)
    {
        re[i] = Strlen % 10 + 48;
        Strlen = Strlen / 10;
    }
    re[4] = '\0';
    if(Strlen > 0)
        return "0000\0";
        return re;
}
int setnonblocking( int _sock)
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