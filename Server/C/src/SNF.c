#include <SNF/SNF.h>
#define _SNF_VER_MAJOR "0"
#define _SNF_VER_MINOR "0"
#define _SNF_VER_PATCH "1"
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