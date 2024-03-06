#ifndef REQUEST_H
#define REQUEST_H

#include <SRBNP/SRBNP.h>
#include <SRBNP/clt.h>
#include <SRBNP/utility.h>
#include <SRBNP/opcode.h>
#include <SRBNP/network.h>

#define NULLREQUEST "000000000000000"
#define SRBNP_REQUEST_MAXSIZE 4096

typedef struct SRBNP_Request_t SRBNP_RQST;
typedef struct SRBNP_Request_args_t SRBNP_RQST_ARG;

struct SRBNP_Request_t
{
    char UID[16];
    SRBNP_opcode *OPCODE;
    SRBNP_RQST_ARG *args;
};

struct SRBNP_Request_args_t
{
    char *arg;
    SRBNP_RQST_ARG *next;
};


extern void srbnp_request_free(SRBNP_RQST *Request);
extern SRBNP_RQST *srbnp_request_gen();
extern SRBNP_RQST *srbnp_request_gen_wUID(const char UID[16]);
extern SRBNP_RQST *srbnp_request_gen_response(SRBNP_RQST *Original, SRBNP_opcode *OPCODE, SRBNP_RQST_ARG *Args);
extern SRBNP_RQST *srbnp_request_gen_server_OPCODE(SRBNP_opcode *OPCODE);
extern SRBNP_RQST *srbnp_request_gen_base(SRBNP_RQST *Original, SRBNP_opcode_mmbr_t Command, SRBNP_opcode_mmbr_t Detail);
extern SRBNP_RQST *srbnp_request_genu_base(SRBNP_RQST *Original, SRBNP_opcode_mmbr_t Command);
extern int srbnp_request_get_nargs(SRBNP_RQST *args);

extern SRBNP_RQST_ARG *srbnp_request_arg_gen(const char* arg);
extern void srbnp_request_arg_free(SRBNP_RQST_ARG *arg);
extern void srbnp_request_args_free(SRBNP_RQST_ARG *arg);
// Only use on first time. unless you dont care about execution time
extern void srbnp_request_arg_insert(SRBNP_RQST *Request, SRBNP_RQST_ARG *arg); 

extern SRBNP_RQST *srbnp_request_fetch(SRBNP_CLT *Client);

extern void srbnp_request_send(SRBNP_CLT *Client, SRBNP_RQST *Request);
extern void srbnp_request_send_confirm(SRBNP_CLT *Client, SRBNP_RQST *Original);
extern void srbnp_request_send_reject(SRBNP_CLT *Client, SRBNP_RQST *Original);
extern void srbnp_request_send_invalid(SRBNP_CLT *Client, SRBNP_RQST *Original);

#endif
