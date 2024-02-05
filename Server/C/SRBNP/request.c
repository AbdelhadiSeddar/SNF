#include "_Imports.h"

SRBNP_RQST *srbnp_request_fetchfrom_clt(SRBNP_CLT *Client)
{
    SRBNP_RQST *re = srbnp_request_gen();
    char MsgSize[5];
    srbnp_rcv(Client, MsgSize, 5);
    printf("Received Content : %s\n", MsgSize);
    int Size = srbnp_Fbyte_TO_int(MsgSize);
    if (Size < 0)
    {
        srbnp_clt_disconnect(Client);
        return NULL;
    }
    char *Request = calloc(Size, sizeof(char));
    srbnp_rcv(Client, Request, Size);
    printf("Received %d Bytes With Content : %s\n", Size, Request);
    strncpy(re->UID, Request, strlen(NULLREQUEST));
    re->UID[strlen(NULLREQUEST)] = '\0';
    char *tmp = calloc(Size - strlen(re->UID), sizeof(char));
    memcpy(tmp, Request + strlen(re->UID), Size -= strlen(re->UID));
    free(Request);
    Request = tmp;

    strncpy(re->OPCODE, Request, 4);
    tmp = calloc(Size - strlen(re->OPCODE), sizeof(char));
    memcpy(tmp, Request + strlen(re->OPCODE), Size -= strlen(re->OPCODE));
    free(Request);
    Request = tmp;

    SRBNP_RQST_ARG *Top_rqst = NULL;
    SRBNP_RQST_ARG *Current = NULL;

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
    srbnp_request_arg_insert(re, Top_rqst);
    return re;
}

SRBNP_RQST *srbnp_request_gen()
{
    SRBNP_RQST *re = malloc(sizeof(SRBNP_RQST));
    re->args = NULL;
    return re;
}

SRBNP_RQST *srbnp_request_gen_wUID(const char UID[16])
{
    SRBNP_RQST *re = srbnp_request_gen();
    memcpy(re->UID, UID, 16);
    return re;
}

void srbnp_request_free(SRBNP_RQST *Request)
{
    if(Request == NULL)
        return;
    srbnp_request_args_free(Request->args);
    free(Request);
}

SRBNP_RQST *srbnp_request_gen_response(SRBNP_RQST *Original, char OPCODE[4], SRBNP_RQST_ARG *Args)
{
    SRBNP_RQST *re = srbnp_request_gen_wUID(Original->UID);
    memcpy(re->OPCODE, OPCODE, 4);
    re->args = Args;
    return re;
}

SRBNP_RQST *srbnp_request_gen_server_OPCODE(char OPCODE[4])
{
    return srbnp_request_gen_response(srbnp_request_gen_wUID(NULLREQUEST), OPCODE, NULL);
}

SRBNP_RQST *srbnp_request_gen_invalid(SRBNP_RQST *Original)
{
    return srbnp_request_gen_response(Original, _OPCODE_CLT_INVALID, NULL);
}

int srbnp_request_get_nargs(SRBNP_RQST *args)
{
    if (args == NULL)
        return 0;
    int nargs = 1;
    SRBNP_RQST_ARG *arg = args->args;
    if(arg == NULL)
        return 0;
    while (arg->next == NULL)
    {
        nargs++;
        arg = arg->next;
    }
    return nargs;
}

SRBNP_RQST_ARG *srbnp_request_arg_gen(const char *arg)
{
    SRBNP_RQST_ARG *Arg = calloc(1, sizeof(SRBNP_RQST_ARG));
    Arg->arg = calloc(strlen(arg) + 1, sizeof(char));
    strcpy(Arg->arg, arg);
    return Arg;
}

void srbnp_request_arg_free(SRBNP_RQST_ARG *arg)
{
    if (arg == NULL)
        return;
    free(arg->arg);
    free(arg);
}

void srbnp_request_args_free(SRBNP_RQST_ARG *arg)
{
    if (arg == NULL)
        return;
    SRBNP_RQST_ARG *current = arg;
    SRBNP_RQST_ARG *next = current ->next;
    while (next != NULL)
    {
        next = current->next;
        srbnp_request_arg_free(current);
        current = next;
    };
    arg = NULL;
}

void srbnp_request_arg_insert(SRBNP_RQST *Request, SRBNP_RQST_ARG *arg)
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

void srbnp_request_send_clt(SRBNP_CLT *Client, SRBNP_RQST *Request)
{
    int nargs = srbnp_request_get_nargs(Request);
    int len = strlen(Request->UID) + nargs * sizeof(char);
    for (SRBNP_RQST_ARG *arg = (Request->args); arg != NULL; arg = arg->next)
        len += strlen(arg->arg);
    char *content = calloc(len, sizeof(char));
    sprintf(content, "%s%s%s",
            srbnp_Fbyte_FROM_int(len),
            Request->UID,
            UNIT_SCEPARATOR);
    int i = 0;
    for (SRBNP_RQST_ARG *arg = (Request->args); arg != NULL; arg = arg->next)
    {
        if (i == 0)
            i = !i;
        else
            strcat(content, UNIT_SCEPARATOR);
        strcat(content, arg->arg);
    }
    srbnp_snd(Client, content, len);
}

void srbnp_request_send_invalid(SRBNP_CLT *Client, SRBNP_RQST *Original)
{
    srbnp_request_send_clt(Client, srbnp_request_gen_invalid(Original));
}
