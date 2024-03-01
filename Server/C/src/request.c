#include <SRBNP/request.h>

SRBNP_RQST *srbnp_request_fetchfrom_clt(
    SRBNP_CLT *Client)
{
    SRBNP_RQST *re = srbnp_request_gen();
    char *Request;

    /**
     * Fetching OPCODE
     */
    srbnp_rcv(Client, re->OPCODE->opcode, 4);

    /**
     * Fetching Request UID
     */
    srbnp_rcv(Client, re->UID, strlen(NULLREQUEST) + 1);

    /**
     * Fetching Arguments Length & handling according to it
     */
    char MsgSize[4];
    srbnp_rcv(Client, MsgSize, 4);
    uint32_t Size = srbnp_bytes_to_uint32(MsgSize, 4);

    if (Size < 0)
        return NULL;
    else if (Size > SRBNP_REQUEST_MAXSIZE)
        return NULL;

    SRBNP_RQST_ARG *Top_rqst = NULL;
    if (Size == 0)
        goto end_request_fetchfrom_clt;
    SRBNP_RQST_ARG *Current = NULL;
    /**
     * Fetching Arguments
     */
    Request = calloc(Size, sizeof(char));
    srbnp_rcv(Client, Request, Size);
    /**
     * Separating Arguments
     */
    while (Size > 0)
    {
        int chr = (int)(strchrnul(Request, UNIT_SCEPARATOR[0]) - Request);
        char *tmpRqst = calloc(chr, sizeof(char));
        strncpy(tmpRqst, Request, chr);
        tmpRqst[chr] = '\0';
        SRBNP_RQST_ARG *new = srbnp_request_arg_gen(tmpRqst);
        if (Top_rqst == NULL)
            Top_rqst = new;
        if (Current == NULL)
            Current = new;
        else
        {
            Current->next = new;
            Current = new;
        }
        char *tmp = calloc(Size - chr - 1, sizeof(char));
        memcpy(tmp, Request + chr + 1, Size -= chr + 1);
        free(Request);
        Request = tmp;
    }
end_request_fetchfrom_clt:;
    srbnp_request_arg_insert(re, Top_rqst);
    return re;
}

SRBNP_RQST *srbnp_request_gen()
{
    SRBNP_RQST *re = calloc(1, sizeof(SRBNP_RQST));
    re->args = NULL;
    re->OPCODE = calloc(1, sizeof(SRBNP_opcode));
    return re;
}

SRBNP_RQST *srbnp_request_gen_wUID(
    const char UID[16])
{
    SRBNP_RQST *re = srbnp_request_gen();
    memcpy(re->UID, UID, 16);
    return re;
}

void srbnp_request_free(
    SRBNP_RQST *Request)
{
    if (Request == NULL)
        return;
    srbnp_request_args_free(Request->args);
    free(Request);
}

SRBNP_RQST *srbnp_request_gen_response(
    SRBNP_RQST *Original,
    SRBNP_opcode *OPCODE,
    SRBNP_RQST_ARG *Args)
{
    SRBNP_RQST *re = srbnp_request_gen_wUID(Original->UID);
    re->OPCODE = OPCODE;
    re->args = Args;
    return re;
}

SRBNP_RQST *srbnp_request_gen_server_OPCODE(
    SRBNP_opcode *OPCODE)
{
    return srbnp_request_gen_response(srbnp_request_gen_wUID(NULLREQUEST), OPCODE, NULL);
}

SRBNP_RQST *srbnp_request_gen_invalid(
    SRBNP_RQST *Original)
{
    return srbnp_request_gen_response(
        Original,
        srbnp_opcode_get_invalid(
            SRBNP_OPCODE_BASE_DET_UNDETAILED),
        NULL);
}

int srbnp_request_get_nargs(
    SRBNP_RQST *args)
{
    if (args == NULL)
        return 0;
    int nargs = 1;
    SRBNP_RQST_ARG *arg = args->args;
    if (arg == NULL)
        return 0;
    while (arg->next == NULL)
    {
        nargs++;
        arg = arg->next;
    }
    return nargs;
}

SRBNP_RQST_ARG *srbnp_request_arg_gen(
    const char *arg)
{
    SRBNP_RQST_ARG *Arg = calloc(1, sizeof(SRBNP_RQST_ARG));
    Arg->arg = calloc(strlen(arg) + 1, sizeof(char));
    strcpy(Arg->arg, arg);
    return Arg;
}

void srbnp_request_arg_free(
    SRBNP_RQST_ARG *arg)
{
    if (arg == NULL)
        return;
    free(arg->arg);
    free(arg);
}

void srbnp_request_args_free(
    SRBNP_RQST_ARG *arg)
{
    if (arg == NULL)
        return;
    SRBNP_RQST_ARG *current = arg;
    SRBNP_RQST_ARG *next = current->next;
    while (next != NULL)
    {
        next = current->next;
        srbnp_request_arg_free(current);
        current = next;
    };
    arg = NULL;
}

void srbnp_request_arg_insert(
    SRBNP_RQST *Request,
    SRBNP_RQST_ARG *arg)
{
    if (Request->args == NULL)
    {
        Request->args = arg;
    }
    else
    {
        SRBNP_RQST_ARG *last = Request->args;
        while (last->next != NULL)
            last = last->next;
        last->next = arg;
    }
}

void srbnp_request_send_clt(
    SRBNP_CLT *Client,
    SRBNP_RQST *Request)
{
    /**
     * Preparing Sending:
     * Getting Arguments Size
     */
    uint32_t Size = 0;
    SRBNP_RQST_ARG *args = Request->args;
    while (args)
    {
        Size += strlen(args->arg);
        args = args->next;
        if (args != NULL)
        {
            Size++;
        }
    }
    char *MsgSize;
    MsgSize = srbnp_uint32_to_bytes(Size, 4);
    /**
     *  Sending OPCODE
     */
    srbnp_snd(Client, (char *)(Request->OPCODE->opcode), 4);
    /**
     * Sending Request UID
     */
    srbnp_snd(Client, Request->UID, strlen(NULLREQUEST) + 1);
    /**
     * Sending Arguments Length
     */
    srbnp_snd(Client, MsgSize, 4);
    free(MsgSize);
    /**
     * Sending Arguments
     */
    if (Size > 0)
    {
        args = Request->args;
        while (args)
        {
            srbnp_snd(Client, args->arg, strlen(args->arg));
            args = args->next;
            if (args)
                srbnp_snd(Client, UNIT_SCEPARATOR, 1);
        }
    }
    srbnp_request_free(Request);
    return;
}

void srbnp_request_send_invalid(
    SRBNP_CLT *Client,
    SRBNP_RQST *Original)
{
    srbnp_request_send_clt(Client, srbnp_request_gen_invalid(Original));
}
