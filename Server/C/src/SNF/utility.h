#ifndef UTILITY_H
#define UTILITY_H

#include <SNF/SNF.h>

extern void (*SNF_ERR_OUT)(const char*);

/// @brief Checks if Result is < 0 then printout the ErrorOut and exits with code 0
/// @param Result Input to be checked if < 0
/// @param ErrorOut Error Message content
extern void checkerr(int Result, const char *ErrorOut);

/// @brief Turns a sequence of bytes into a uint32 (Following big-endian)
/// @param bytes Bytes sequence
/// @param nBytes Bytes sequence's length
/// @return bytes's value in uint32_t On Sucess || 0
extern uint32_t snf_bytes_to_uint32(const char *bytes, int nBytes);

/// @brief Turns a string's length into a sequence of bytes (Following big-endian)
/// @param Str String whose length would be formatted into Bytes sequence
/// @param nBytes Bytes sequence's length
/// @return Byte sequence On Success | NULL On fail { Shall fail if Str is NULL || nBytes is a negative number || Str doesn't finish with a null character  || an Error occured during allocation }
extern char *snf_strlen_to_bytes(const char *Str, int nBytes);

/// @brief Turns a uint32 into a sequence of bytes (Following big-endian)
/// @param i uint32_t that would be formatted into Bytes sequence
/// @param nBytes Bytes sequence's length
/// @return Byte sequence On Success | NULL On fail { Shall fail if nBytes is a negative number || Str doesn't finish with a null character  || an Error occured during allocation }
extern char *snf_uint32_to_bytes(uint32_t i, int nBytes);

/// @brief Sets a socket as non blocking.
/// @param _sock Socket to be operated on
/// @return See fcntl(..)
extern int snf_setnonblocking(int _sock);


extern void snf_print_err(const char* StrErr);

#endif