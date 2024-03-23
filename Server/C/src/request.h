#ifndef REQUEST_H
#define REQUEST_H

#include <SNF/SNF.h>
#include <SNF/clt.h>
#include <SNF/utility.h>
#include <SNF/opcode.h>
#include <SNF/network.h>

#define NULLREQUEST "000000000000000"
#define SNF_REQUEST_MAXSIZE 4096

typedef struct SNF_Request_t SNF_RQST;
typedef struct SNF_Request_args_t SNF_RQST_ARG;

struct SNF_Request_t
{
    char UID[16];
    SNF_opcode *OPCODE;
    SNF_RQST_ARG *args;
};

struct SNF_Request_args_t
{
    char *arg;
    SNF_RQST_ARG *next;
};

extern void snf_request_free(SNF_RQST *Request);
extern SNF_RQST *snf_request_gen();
extern SNF_RQST *snf_request_gen_wUID(const char UID[16]);
extern SNF_RQST *snf_request_gen_response(SNF_RQST *Original, SNF_opcode *OPCODE, SNF_RQST_ARG *Args);
extern SNF_RQST *snf_request_gen_server_OPCODE(SNF_opcode *OPCODE);
extern SNF_RQST *snf_request_gen_base(SNF_RQST *Original, SNF_opcode_mmbr_t Command, SNF_opcode_mmbr_t Detail);
extern SNF_RQST *snf_request_genu_base(SNF_RQST *Original, SNF_opcode_mmbr_t Command);
extern int snf_request_get_nargs(SNF_RQST *args);

extern SNF_RQST_ARG *snf_request_arg_gen(const char *arg);
extern void snf_request_arg_free(SNF_RQST_ARG *arg);
extern void snf_request_args_free(SNF_RQST_ARG *arg);
// Only use on first time. unless you dont care about execution time
extern void snf_request_arg_insert(SNF_RQST *Request, SNF_RQST_ARG *arg);

extern SNF_RQST *snf_request_fetch(SNF_CLT *Client);

extern void snf_request_send(SNF_CLT *Client, SNF_RQST *Request);
extern void snf_request_send_confirm(SNF_CLT *Client, SNF_RQST *Original);
extern void snf_request_send_reject(SNF_CLT *Client, SNF_RQST *Original);
extern void snf_request_send_invalid(SNF_CLT *Client, SNF_RQST *Original);

#endif
