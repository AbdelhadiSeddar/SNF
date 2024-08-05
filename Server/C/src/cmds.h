////////////////////////////////////////////////////////////////////
///
/// \file cmds.h
/// This file is used to define functions that would be called
/// either by default or Base Command OPCode's Functions
///
///
///
/// \warning    Do not call any function defined here manually
///
/////////////////////////////////////////////////////////////////// 


#include <SNF/SNF.h>
#include <SNF/request.h>

extern SNF_RQST *snf_cmd_invalid_error_protocol(SNF_RQST *);
extern SNF_RQST *snf_cmd_invalid_unregistred(SNF_RQST *);
extern SNF_RQST *snf_cmd_invalid_unimplemented(SNF_RQST *);

extern SNF_RQST *snf_cmd_snf_ver(SNF_RQST *);
