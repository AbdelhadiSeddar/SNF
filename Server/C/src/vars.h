#ifndef snf_vars_h
#define snf_vars_h


#include <SNF/SNF.h>

typedef enum SNF_VARS_e {
    SNF_VAR_THREADS,
    SNF_VAR_THREADPOOL,
    SNF_VAR_PORT,
    SNF_VAR_MAX_QUEUE,
    SNF_VAR_OPCODE_INIS,
    SNF_VAR_EPOLL_MAXEVENTS,
    SNF_VAR_EPOLL_EVENTS,
    SNF_VAR_EPOLL_TIMEOUT,
    SNF_VAR_CLTS_INITIAL,
    SNF_VAR_RQST_MAX_LENGTH,
    SNF_VAR_INITIALIZED,
} SNF_VARS;

#define SNF_N_VARS ((int)SNF_VAR_INITIALIZED + 1) 

extern void snf_var_default();

/// @brief 
/// @param VARNAME 
/// @return 
extern void **snf_var_geta_void(SNF_VARS VARNAME);
#define snf_var_geta(VARNAME, TYPE) ((TYPE **)snf_var_geta_void(VARNAME))
#define snf_var_get(VARNAME, TYPE) *snf_var_geta(VARNAME, TYPE)
#define snf_var_getv(VARNAME, TYPE) *snf_var_get(VARNAME, TYPE)
extern void snf_var_set(SNF_VARS VARNAME, void* Value);

#include <SNF/thpool.h>
#endif