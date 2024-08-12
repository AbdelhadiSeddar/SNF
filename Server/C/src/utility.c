#include <SNF/utility.h>
#include "utility.h"

void (*SNF_ERR_OUT)(const char*) = snf_print_err;

void checkerr(int Result, const char *ErrorOut)
{
    if (Result < 0)
    { 
        SNF_ERR_OUT(ErrorOut);
    }
}
uint32_t snf_bytes_to_uint32(const char *bytes, int nBytes)
{
    if (bytes == NULL || nBytes < 0)
        return -1;
    uint32_t re = 0;
    for (int i = 0; i < nBytes; i++)
    {
        re += (bytes[nBytes - i - 1] << (8 * i));
    }
    return re;
}
char *snf_strlen_to_bytes(const char *Str, int nBytes)
{
    if (Str == NULL || nBytes < 0)
        return NULL;
    int len = 0;
    if ((len = strnlen(Str, 0)))
        return snf_uint32_to_bytes(len, nBytes);
    else
        return NULL;
}
char *snf_uint32_to_bytes(uint32_t i32, int nBytes)
{
    if (nBytes < 0)
        return NULL;
    char *re = calloc(sizeof(char), nBytes);
    for (int i = 0; i < nBytes; i++)
    {
        re[nBytes - i - 1] = (i32 >> 8 * i) & 0xFF;
    }
    return re;
}
int snf_setnonblocking(int _sock)
{
    int result;
    int flags;

    flags = fcntl(_sock, F_GETFL, 0);

    if (flags == -1)
    {
        return -1; // error
    }

    flags |= O_NONBLOCK;

    result = fcntl(_sock, F_SETFL, flags);
    return result;
}

void snf_print_err(const char *StrErr)
{
    perror(StrErr);
    exit(EXIT_FAILURE);
}
