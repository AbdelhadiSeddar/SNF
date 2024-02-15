#include <SRBNP/utility.h>

void checkerr(int Result, const char *ErrorOut)
{
    if (Result < 0)
    {
        perror(ErrorOut);
        exit(EXIT_FAILURE);
    }
}
uint32_t srbnp_bytes_to_uint32(const char *bytes, int nBytes)
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
char *srbnp_strlen_to_bytes(const char *Str, int nBytes)
{
    if (Str == NULL || nBytes < 0)
        return NULL;
    int len = 0;
    if ((len = strnlen(Str, 0)))
        return srbnp_uint32_to_bytes(len, nBytes);
    else
        return NULL;
}
char *srbnp_uint32_to_bytes(uint32_t i32, int nBytes)
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
int srbnp_setnonblocking(int _sock)
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
