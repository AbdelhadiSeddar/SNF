#ifndef NETWORK_H
#define NETWORK_H

#define _SNF_VER "0.0.1-alpha"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <strings.h>
#include <arpa/inet.h>

#include <SNF/SNF.h>
#include <SNF/clt.h>
#include <SNF/thpool.h>

extern thpool *Ntwrk;
extern int SNF_PORT;
extern int SNF_MAX_QUEUE;

extern int SNF_SERVER_SOCKET;
extern struct sockaddr_in SNF_SERVER_ADDR;
extern struct sockaddr_in SNF_CLIENT_ADDR;
extern socklen_t SNF_CLIENT_LEN;

#define SNF_Total_Data (SNF_Total_Data_Rcv + SNF_Total_Data_Snt)
extern _Atomic uint64_t SNF_Total_Data_Rcv;
extern _Atomic uint64_t SNF_Total_Data_Snt;

#define UNIT_SCEPARATOR "\x1F"

extern int SNF_SERVER_SOCKET;
extern void snf_network_init();
extern void snf_network_join();

extern int snf_snd(SNF_CLT *Client, const char *Buffer, int _Size);
extern int snf_rcv_(SNF_CLT *Client, void *Buffer, int _Size, int _Flags);
#define snf_rcv(Client, Buffer, _Size) snf_rcv_(Client, Buffer, _Size, 0)
#define snf_rcv_PEEK(Client, Buffer, _Size) snf_rcv_(Client, Buffer, _Size, MSG_PEEK)

#endif