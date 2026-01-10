#include "SNF/SNF.h"


#define _SNF_VER_MAJOR "0"
#define _SNF_VER_MINOR "0"
#define _SNF_VER_PATCH "3"
#define _SNF_VER_EXTRA "-alpha"

#define _SNF_SNP_VER_MAJOR "0"
#define _SNF_SNP_VER_MINOR "0"
#define _SNF_SNP_VER_PATCH "1"
#define _SNF_SNP_VER_EXTRA ""

char* snf_getver()
{
  return 
    _SNF_VER_MAJOR"."
    _SNF_VER_MINOR"."
    _SNF_VER_PATCH
    _SNF_VER_EXTRA
  ;
}
char* snf_getsnpver()
{
  return 
    _SNF_SNP_VER_MAJOR"."
    _SNF_SNP_VER_MINOR"."
    _SNF_SNP_VER_PATCH
    _SNF_SNP_VER_EXTRA
  ;
}
void snf_default_error_print(char * Error)
{
    fprintf(stderr, "%s", Error);
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
