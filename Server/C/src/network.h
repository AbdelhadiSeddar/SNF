//////////////////////////////////////////////////////////////
///
/// \file network.h
/// This file Defines everything related to networking 
///
/// //////////////////////////////////////////////////////////
#ifndef NETWORK_H
#define NETWORK_H


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <strings.h>
#include <arpa/inet.h>

#include <SNF/SNF.h>
#include <SNF/clt.h>
#include <SNF/thpool.h>
#include <SNF/vars.h>


/// @brief the main Socket that accepts new incoming connections
/// @warning Do not modify it's contents unless you know what you're doing.
extern int SNF_SERVER_SOCKET;
/// @brief Structure that saves the Server's sockaddr_in
/// @warning Do not modify it's contents unless you know what you're doing.
extern struct sockaddr_in SNF_SERVER_ADDR;
/// @brief Structure that saves the Client's sockaddr_in
/// @warning Do not modify it's contents unless you know what you're doing.
extern struct sockaddr_in SNF_CLIENT_ADDR;
/// @brief Used for accept's length argument.
/// @warning Do not modify it's contents unless you know what you're doing.
extern socklen_t SNF_CLIENT_LEN;

/// @brief Macro that defines the Total data tranferred so far
/// @warning Innacurate for now
#define SNF_Total_Data (SNF_Total_Data_Rcv + SNF_Total_Data_Snt)
/// @brief Saves the Total data received so far
/// @warning Innacurate for now
extern _Atomic uint64_t SNF_Total_Data_Rcv;
/// @brief Saves the Total data sent so far
/// @warning Innacurate for now
extern _Atomic uint64_t SNF_Total_Data_Snt;

/// @brief Character that sceparates each argument (To be Removed)
#define UNIT_SCEPARATOR "\x1F"

/// @brief Initializes the Network Framwork
extern void snf_network_init();
/// @brief See snf_thpool_join 
extern void snf_network_join();

/// @brief Sends a Buffer to Client's socket's file descriptor
/// @param Client Pointer to the Client's SNF_CLT instance
/// @param Buffer Buffer to send
/// @param _Size Size of the Buffer (See Note )
/// @return The following possible results:
///         * **-1** **Client** Could be NULL, or In case of an Error , Check **errno** and compare it to **send**'s possible Errors, if errno == EPIPE then ignore it as it was already dealt with.
///         * **int** The amount of data sent.
/// @note The <strong>_Size</strong> parameter could react differently depeding on it's value
///         * If <strong>_Size</strong> < 0 then it will assume the **Buffer** is a string and send 4 bytes indicating the size (See \ref snf_uint32_to_bytes on how those 4 bytes are written ) and then send the buffer.
///         * else it will send the **Buffer** and the length will be <strong>_Size</strong>
extern int snf_snd(SNF_CLT *Client, const char *Buffer, int _Size);

/// @brief Receives a Buffer from Client's socket's file descriptor
/// @param Client Pointer to the Client's SNF_CLT instance
/// @param Buffer Received Buffer 
/// @param _Size Expected Size 
/// @param _Flags Flags that will be passed to recv()
/// @return The following possible results:
///         * **-1** **Client** Could be NULL, or In case of an Error , Check **errno** and compare it to **send**'s possible Errors, if errno == EPIPE then ignore it as it was already dealt with.
///         * **int** The amount of data sent.
/// @warning If you receive *n* amount of bytes and is lower than _Size , it will assume that recv has failed and will check errno, as it could disconnect the client in somecases and the function will return **-1**.
/// \note   There is are re-definitions of snf_recv_ and are:
///         * snf_rcv() 
///         * snf_rcv_PEEK()
extern int snf_rcv_(SNF_CLT *Client, void *Buffer, int _Size, int _Flags);
///@brief Same as \ref snf_rcv_ how ever <strong>_Flags</strong> will be 0 
#define snf_rcv(Client, Buffer, _Size) snf_rcv_(Client, Buffer, _Size, 0)
///@brief Same as \ref snf_rcv_ how ever <strong>_Flags</strong> will be MSG_PEEK
#define snf_rcv_PEEK(Client, Buffer, _Size) snf_rcv_(Client, Buffer, _Size, MSG_PEEK)

#endif