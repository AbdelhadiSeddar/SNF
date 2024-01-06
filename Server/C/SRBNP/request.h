#ifndef REQUEST_H
#define REQUEST_H
#include "clt.h"

#define NULLREQUEST "000000000000000"

typedef struct Request_t Rqst;
typedef struct Request_args_t Rqst_arg;

struct Request_t
{
    char UID[16];
    char OPCODE[4];
    Rqst_arg *args;
};

struct Request_args_t
{
    char *arg;
    Rqst_arg *next;
};

extern Rqst *request_fetchfrom_clt(Clt *Client);

extern Rqst *request_gen();
extern Rqst *request_gen_wUID(const char UID[16]);
extern void request_free(Rqst *Request);
extern Rqst *request_gen_response(Rqst *Original, char OPCODE[4], Rqst_arg *Args);
extern Rqst *request_gen_server_OPCODE(char OPCODE[4]);
extern Rqst *request_gen_invalid(Rqst *Original);
extern int request_get_nargs(Rqst *args);

extern Rqst_arg *request_arg_gen(const char* arg);
extern void request_arg_free(Rqst_arg *arg);
extern void request_args_free(Rqst_arg *arg);
extern void request_arg_insert(Rqst *Request, Rqst_arg *arg); // Only use on first time. unless you dont care about execution time

extern void request_send_clt(Clt *Client, Rqst *Request);
extern void request_send_invalid(Clt *Client, Rqst *Original);

#endif
