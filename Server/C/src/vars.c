     #define xstr(s) str(s)
     #define str(s) #s
#include "SNF/vars.h"

void *SNF_VARS_TABLE[SNF_N_VARS];
size_t SNF_VARS_TABLE_LENGTHS[SNF_N_VARS];

#define alloc_vartable(INDEX, TYPE) \
    SNF_VARS_TABLE_LENGTHS[INDEX] = sizeof(TYPE); \
    (*(TYPE *)(SNF_VARS_TABLE[INDEX] = calloc(1, sizeof(TYPE))))

void snf_var_default()
{
    alloc_vartable(SNF_VAR_THREADS, int) = 4 ;
    alloc_vartable(SNF_VAR_THREADPOOL, SNF_thpool);
    alloc_vartable(SNF_VAR_PORT, int) = 9114;
    alloc_vartable(SNF_VAR_MAX_QUEUE, int) = 1000;
    alloc_vartable(SNF_VAR_OPCODE_INIS, int) = 0;
    alloc_vartable(SNF_VAR_EPOLL_MAXEVENTS, int) = 4096;
    alloc_vartable(SNF_VAR_EPOLL_EVENTS, struct epoll_event *) 
    = calloc(snf_var_getv(SNF_VAR_EPOLL_MAXEVENTS, int), sizeof(struct epoll_event));
    alloc_vartable(SNF_VAR_EPOLL_TIMEOUT, _Atomic int) = 10;
    alloc_vartable(SNF_VAR_CLTS_INITIAL, int) = 100;
    alloc_vartable(SNF_VAR_RQST_MAX_LENGTH, int) = 4096;
    alloc_vartable(SNF_VAR_INITIALIZED, int) = 1;
}

void **snf_var_geta_void(SNF_VARS VARNAME)
{
    if(VARNAME < 0 || VARNAME > SNF_VAR_INITIALIZED)
        return NULL;
    return &(SNF_VARS_TABLE[VARNAME]);
}

void snf_var_set(SNF_VARS VARNAME, void *Value)
{
    if(VARNAME < 0 || VARNAME > SNF_VAR_INITIALIZED || Value == NULL)
        return;
    char *ets= NULL;
    int def = 0;
    switch(VARNAME)
    {
        // 0 is limit
        case SNF_VAR_PORT:
            if(!def) { def++ ; ets = xstr(SNF_VAR_PORT); }
        case SNF_VAR_MAX_QUEUE:
            if(!def) { def++ ; ets = xstr(SNF_VAR_MAX_QUEUE); }
        case SNF_VAR_EPOLL_MAXEVENTS:
            if(!def) { def++ ; ets = xstr(SNF_VAR_EPOLL_MAXEVENTS); }
        case SNF_VAR_CLTS_INITIAL:
            if(!def) { def++ ; ets = xstr(SNF_VAR_CLTS_INITIAL); }
            if(*(int *)Value < 0)
                fprintf(stderr, "Invalid Value %d, Value must be >= 0 for variable %s. IGNORED!\n", *(int*)Value, ets);
            break;

        // -1 is limit
        case SNF_VAR_EPOLL_TIMEOUT:
            if(*(int *)Value < -1)
                fprintf(stderr, "Invalid Value %d, Value must be >= -1 for variable %s. IGNORED!\n", *(int*)Value, xstr(SNF_VAR_EPOLL_TIMEOUT));
            break;
        // 3 is limit
        case SNF_VAR_EPOLL_TIMEOUT:
            if(*(int *)Value < 3)
                fprintf(stderr, "Invalid Value %d, Value must be >= 3 for variable %s. IGNORED!\n", *(int*)Value, xstr(SNF_VAR_THREADS));
            break;
        // 64 is limit
        case SNF_VAR_RQST_MAX_LENGTH:
            if(*(int *)Value < 64)
                fprintf(stderr, "Invalid Value %d, Value must be >= 64 for variable %s. IGNORED!\n", *(int*)Value, xstr(SNF_VAR_RQST_MAX_LENGTH));
            break;
        // 0 or 1 is limit
        case SNF_VAR_OPCODE_INIS:
            if(*(int *)Value != 0 && *(int *)Value != 1)
                fprintf(stderr, "Invalid Value %d, Value must be either 0(False) or 1(True) for variable %s. IGNORED!\n", *(int*)Value, xstr(SNF_VAR_OPCODE_INIS));
            break;
        default:
            break;
    }
    switch(VARNAME)
    {
        #pragma region Handling SNF_VAR_EPOLL_*
            case SNF_VAR_EPOLL_MAXEVENTS:
                SNF_VARS_TABLE[VARNAME]=Value;
                free(SNF_VARS_TABLE[SNF_VAR_EPOLL_EVENTS]);
                alloc_vartable(SNF_VAR_EPOLL_EVENTS, struct epoll_event *) 
                = calloc(snf_var_getv(SNF_VAR_EPOLL_MAXEVENTS, int), sizeof(struct epoll_event));
                break;
            case SNF_VAR_EPOLL_EVENTS:
                fprintf(stderr, "Cannot set SNF_VAR_EPOLL_EVENTS, IGNORED!\n. Use SNF_VAR_EPOLL_MAXEVENTS.");
                break;
        #pragma endregion

        case SNF_VAR_INITIALIZED:
            fprintf(stderr, "Cannot set SNF_VAR_INITIALIZED, IGNORED!\n.");
            break;

        default:
            SNF_VARS_TABLE[VARNAME] = Value;
            break;
    }
}
