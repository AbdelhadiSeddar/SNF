#ifndef NETWORK_H
#define NETWORK_H

#define _SRBNP_VER 0000

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <SRBNP/SRBNP.h>
#include <SRBNP/clt.h>
#include <SRBNP/thpool.h>

extern thpool *Ntwrk;
extern int _PORT;
extern int _MAX_QUEUE;

extern _Atomic uint64_t SRBNP_Total_Data_Rcv;
extern _Atomic uint64_t Total_Data_Snt;

extern int _SERVER_SOCKET;
extern struct sockaddr_in _SERVER_ADDR;
extern struct sockaddr_in _CLIENT_ADDR;
extern socklen_t _CLIENT_LEN;


#define SRBNP_Total_Data  (Total_Data_Rcv + Total_Data_Snt)
extern _Atomic uint64_t SRBNP_Total_Data_Rcv;
extern _Atomic uint64_t SRBNP_Total_Data_Snt;

#define UNIT_SCEPARATOR "\x1F"

extern int _SERVER_SOCKET;
extern void network_init();


extern int srbnp_snd(SRBNP_CLT *Client, const char *Buffer, int _Size);
extern int srbnp_rcv_(SRBNP_CLT *Client, void *Buffer, int _Size, int _Flags);
#define srbnp_rcv(Client, Buffer, _Size) srbnp_rcv_(Client, Buffer, _Size, 0)
#define srbnp_rcv_PEEK(Client, Buffer, _Size) srbnp_rcv_(Client, Buffer, _Size, MSG_PEEK)
extern int srbnp_handle_zombie(int sock);

#endif