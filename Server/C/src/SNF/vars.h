////////////////////////////////////////////////////////////////////////////////
///
/// \brief this file Defines everything related to Globaly accessed variable
/// \file vars.h
///
/// This file serves as a way to put the variables that are required to be 
/// globally accessed, as this will ease for developper's access to them 
/// and store  them in one place instead of them being scattered in different
/// files.
///
/// \note <br>
/// Headers that are cable of working independently ( aka 
/// lookup3.h, hashtable.h and thpool.h are unaffected by these variables)
///  do not require to default the variables if you're just using those headers
////////////////////////////////////////////////////////////////////////////////
#ifndef snf_vars_h
#define snf_vars_h


#include <SNF/SNF.h>

/// @brief This Enum saved the identifiers for variables available as of this version
/// @note If you set a value lower than the **Minimum Value** or that value is not in the **Allowed Values** an error will be printed and the value will not be changed!
typedef enum SNF_VARS_e {
    /// @brief Variable for threads used by SNF 
    /// @line 
    /// 
    /// **Type**: int <br>
    /// **Default Value**: 4 <br>
    /// **Minimum Value**: 3 <br>
    /// @warning Do not change after starting snf server
    SNF_VAR_THREADS,
    /// @brief Variable for saving the threadpool instance
    /// @line 
    /// 
    /// **Type**: SNF_thpool <br>
    /// **Default Value**:  Default Instance of SNF_thpool<br>
    /// @warning Never Set/Change this variable manually
    SNF_VAR_THREADPOOL,
    /// @brief Variable for saving the port used by SNF
    /// @line 
    /// 
    /// **Type**: int <br>
    /// **Default Value**: 9114<br>
    /// **Minimum Value**: 0 <br>
    SNF_VAR_PORT,
    /// @brief Variable for saving maximum connections that are able to queue while waiting forthe server to accept, (aka passed as a parameter to listen()) 
    /// @line 
    /// 
    /// **Type**: int <br>
    /// **Default Value**: 1000<br>
    /// **Minimum Value**: 0 <br>
    SNF_VAR_MAX_QUEUE,
    /// @brief Variable for saving if SNF_VAR_OPCODE_INIS
    /// @line 
    /// 
    /// **Type**: int <br>
    /// **Default Value**: 0<br>
    /// **Allowed Values**: 0, 1<br>
    /// @note Ignored, use \ref SNF_opcode_base_isinit to Check if the opcode is initialized or not.
    SNF_VAR_OPCODE_INIS,
    /// @brief Variable for saving the amount possible of concurrent connections ( See limitations of epoll )
    /// @line 
    /// 
    /// **Type**: int <br>
    /// **Default Value**: 4096<br>
    /// **Minimum Value**: 0<br>
    SNF_VAR_EPOLL_MAXEVENTS,
    /// @brief Variable for saving events that are used by epoll.
    /// 
    /// **Type**: struct event_epoll *
    /// 
    /// **Value**: Table of [(struct event_epoll) x (value of SNF_VAR_EPOLL_MAXEVENTS)]
    /// 
    /// @note Trying to set this value will print an error and will be ignored.<br>
    /// The Value of this variable is set using the calculation stated in **Value** when you set SNF_VAR_EPOLL_MAXEVENTS 
    SNF_VAR_EPOLL_EVENTS,
    /// @brief Variable for saving the maximum timeout in miliseconds for epoll_wait()
    /// @line 
    /// 
    /// **Type**: _Atomic int <br>
    /// **Default Value**: 10<br>
    /// **Minimum Value**: -1 <br>
    SNF_VAR_EPOLL_TIMEOUT,
    /// @brief Variable for the amount of initially planned amount of client to connect, for the Hashtable saving the clients
    /// @line
    /// 
    /// **Type**: int <br>
    /// **Default Value**: 100<br>
    /// **Minimum Value**: 0 <br> 
    /// @note It will be changed where the actual hashtable length equals to 2^n where **n** will be the lowest value that makes the 2^n fit the value set for this variable <br>
    /// **eg**: If you set SNF_VAR_CLTS_INITIAL's value to 100, the actual hashtable length will be 128 
    ///
    SNF_VAR_CLTS_INITIAL,
    /// @brief Variable for defining the storage size of the extra data. If the value equals or less than 0 it means it has no extra data to save
    /// @line 
    ///  
    /// **Type**: size_t <br>
    /// **Default Value**:  -1<br>
    /// **Minimum Value**:  -1<br>
    SNF_VAR_CLTS_DATA_SIZE,
    /// @brief Variable for definiting the functions to call upon certain events of the client.
    /// @line 
    /// Events are 
    ///   * On Connect  (Function Type: int on_connect(SNF_CLT*))
    ///   * On Accept   (Function Type: void on_accept(SNF_CLT*))
    ///
    /// **Type**: SNF_CLT_HANLDER *<br>
    /// **Default Value**:  NULL<br>
    SNF_VAR_CLTS_HANDLERS,
    ///
    /// @brief Variable for saving the maximum allowed length of a Request 
    /// @line 
    /// 
    /// **Type**: int <br>
    /// **Default Value**: 4096<br>
    /// **Minimum Value**: 0<br>
    SNF_VAR_RQST_MAX_LENGTH,
    /// @brief Variable for saving if the Variables has been defaulted and/or initalized before.
    /// @line 
    /// 
    /// **Type**: int <br>
    /// **Default Value**: 0<br>
    /// **Allowed Values**: 0, 1<br>
    /// @note Trying to set this variable will print an error and will be ignored 
    SNF_VAR_INITIALIZED,
} SNF_VARS;

/// @brief Returns the amount of variables registred in this structure
#define SNF_N_VARS ((int)SNF_VAR_INITIALIZED + 1) 
/// @brief Defaults and allocates the variables 
extern void snf_var_default();

/// @brief Returns the address of the void* pointer of the variable
/// @param VARNAME Variable's Idnetifier 
/// @return 
extern void **snf_var_geta_void(SNF_VARS VARNAME);
/// @brief The same as snf_var_geta_void but casts the return from (void **) into (TYPE **)
/// @param VARNAME Variable's identifier
/// @param TYPE Variable's pointer's type
#define snf_var_geta(VARNAME, TYPE) ((TYPE **)snf_var_geta_void(VARNAME))
/// @brief Derenferences the return of snf_var_geta from TYPE** into TYPE *
/// @param VARNAME Variable's identifier
/// @param TYPE Variable's pointer type
/// @note Best in case the variable's type is a pointer 
#define snf_var_get(VARNAME, TYPE) *snf_var_geta(VARNAME, TYPE)
/// @brief Dereferences the return of snf_var_get from TYPE * to TYPE 
/// @param VARNAME Variable's identifier
/// @param TYPE Variable's Type
/// @note Best in case your type was not registred as  a pointer ( eg: TYPE == int )
#define snf_var_getv(VARNAME, TYPE) *snf_var_get(VARNAME, TYPE)
/// @brief Allows to set the value of a variable
/// @param VARNAME Variable's identifier
/// @param Value Variable's new value
/// @warning Make sure you dont change certain values on unappropriate moments, Check \ref SNF_VARS
extern void snf_var_set(SNF_VARS VARNAME, void* Value);

#include <SNF/thpool.h>
#endif
