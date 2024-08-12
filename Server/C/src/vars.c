#include <SNF/vars.h>


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

    SNF_VARS_TABLE[VARNAME] = Value;
}
