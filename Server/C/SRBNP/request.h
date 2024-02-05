#ifndef REQUEST_H
#define REQUEST_H
#include "clt.h"

#define NULLREQUEST "000000000000000"

typedef struct SRBNP_Request_t SRBNP_RQST;
typedef struct SRBNP_Request_args_t SRBNP_RQST_ARG;

struct SRBNP_Request_t
{
    char UID[16];
    char OPCODE[4];
    SRBNP_RQST_ARG *args;
};

struct SRBNP_Request_args_t
{
    char *arg;
    SRBNP_RQST_ARG *next;
};

extern SRBNP_RQST *srbnp_request_fetchfrom_clt(SRBNP_CLT *Client);

extern void srbnp_request_free(SRBNP_RQST *Request);
extern SRBNP_RQST *srbnp_request_gen();
extern SRBNP_RQST *srbnp_request_gen_wUID(const char UID[16]);
extern SRBNP_RQST *srbnp_request_gen_response(SRBNP_RQST *Original, char OPCODE[4], SRBNP_RQST_ARG *Args);
extern SRBNP_RQST *srbnp_request_gen_server_OPCODE(char OPCODE[4]);
extern SRBNP_RQST *srbnp_request_gen_invalid(SRBNP_RQST *Original);
extern int srbnp_request_get_nargs(SRBNP_RQST *args);

extern SRBNP_RQST_ARG *srbnp_request_arg_gen(const char* arg);
extern void srbnp_request_arg_free(SRBNP_RQST_ARG *arg);
extern void srbnp_request_args_free(SRBNP_RQST_ARG *arg);
// Only use on first time. unless you dont care about execution time
extern void srbnp_request_arg_insert(SRBNP_RQST *Request, SRBNP_RQST_ARG *arg); 

extern void srbnp_request_send_clt(SRBNP_CLT *Client, SRBNP_RQST *Request);
extern void srbnp_request_send_invalid(SRBNP_CLT *Client, SRBNP_RQST *Original);

#endif
