#ifndef NETWORK_H
#define NETWORK_H
#include "clt.h"
/// Temporary Verion Number

#define Total_Data  (Total_Data_Rcv + Total_Data_Snt)
extern _Atomic uint64_t Total_Data_Rcv;
extern _Atomic uint64_t Total_Data_Snt;

#define UNIT_SCEPARATOR "\x1F"

extern int _SERVER_SOCKET;
extern void network_init();


extern int snd(Clt *Client, const char *Buffer, int _Size);
extern int rcv_(Clt *Client, void *Buffer, int _Size, int _Flags);
#define rcv(Client, Buffer, _Size) rcv_(Client, Buffer, _Size, 0)
#define rcv_PEEK(Client, Buffer, _Size) rcv_(Client, Buffer, _Size, MSG_PEEK)
extern int network_handle_zombie(int sock);

#endif