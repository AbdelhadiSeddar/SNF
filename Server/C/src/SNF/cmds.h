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
/// @brief Called when an opcode command has been called by the client that must'nt be called 
/// @param  SNF_RQST Client's Original Request
/// @return The Request to be Sent
extern SNF_RQST *snf_cmd_invalid_error_protocol(SNF_RQST *);
/// @brief Called when the client has called an opcode that isn't registred on the server
/// @param  SNF_RQST Client's Original Request
/// @return The Request to be Sent
extern SNF_RQST *snf_cmd_invalid_unregistred(SNF_RQST *);
/// @brief Called when the client has called a registred opcode, but has no function to call in the server
/// @param  SNF_RQST Client's Original Request
/// @return The Request to be Sent
extern SNF_RQST *snf_cmd_invalid_unimplemented(SNF_RQST *);

/// @brief Called when the client requests the server's version
/// @param  SNF_RQST Client's Original Request
/// @return The Request to be Sent
extern SNF_RQST *snf_cmd_ver(SNF_RQST *);

/// @brief Called when the client wants to be disconnected
/// Though is just a placeholder
///
/// @param SNF_RQST Client's Original Request
/// @return The Requesy to be Sent
extern SNF_RQST *snf_cmd_disconnect(SNF_RQST *);
