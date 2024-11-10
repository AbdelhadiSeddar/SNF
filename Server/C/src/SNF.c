#include "SNF/SNF.h"
#define _SNF_VER_MAJOR "0"
#define _SNF_VER_MINOR "0"
#define _SNF_VER_PATCH "2"
#define _SNF_VER_EXTRA "-alpha"


char* snf_getver()
{
    int size = strlen(_SNF_VER_MAJOR);
    size += strlen(_SNF_VER_MINOR);
    size += strlen(_SNF_VER_PATCH);
    size += strlen(_SNF_VER_EXTRA);

    size += 3;

    char*re = calloc(size, sizeof(char));

    snprintf(re, size, "%s.%s.%s%s", 
        _SNF_VER_MAJOR,
        _SNF_VER_MINOR,
        _SNF_VER_PATCH,
        _SNF_VER_EXTRA
    );
    return re;
}

void snf_default_error_print(char * Error)
{
    fprintf(stderr, "%s", char* Error);
}

void snf_error_print(char* format, ...){
    va_list args;
    char *str = calloc(250, sizeof(char));
    va_start(args, format);
    vsnprintf(str, 250, format, args);
    va_end(args);

    snf_default_error_print(str);

    free(str);
    return;
}