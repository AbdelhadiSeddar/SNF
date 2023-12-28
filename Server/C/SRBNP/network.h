#ifndef NETWORK_H
#define NETWORK_H

#define Total_Data  (Total_Data_Rcv + Total_Data_Snt)
extern _Atomic uint64_t Total_Data_Rcv;
extern _Atomic uint64_t Total_Data_Snt;

extern int _SERVER_SOCKET;
extern void network_init();


extern int snd(int Socket, const char* Buffer, int _Size);
extern int rcv(int Socket, void* Buffer, int _Size);

#endif