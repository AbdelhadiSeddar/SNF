#include "_Imports.h"
#include "request.h"

Rqst *request_fetchfrom_clt(Clt *Client)
{
    Rqst *re = request_gen();
    char MsgSize[5];
    rcv(Client, MsgSize, 5);
    int Size = Fbyte_TO_int(MsgSize);
    if (Size < 0)
    {
        request_send_invalid(Client, request_gen_wUID(NULLREQUEST));
        return NULL;
    }
    char *Request = calloc(Size, sizeof(char));
    rcv(Client, Request, Size);

    strncpy(re->UID, Request, 16);
    re->UID[15] = '\0';
    char *tmp = calloc(Size - strlen(re->UID), sizeof(char));
    memcpy(tmp, Request + strlen(re->UID), Size -= strlen(re->UID));
    free(Request);
    Request = tmp;

    strncpy(re->OPCODE, Request, 4);
    tmp = calloc(Size - strlen(re->OPCODE), sizeof(char));
    memcpy(tmp, Request + strlen(re->OPCODE), Size -= strlen(re->OPCODE));
    free(Request);
    Request = tmp;

    Rqst_arg *Top_rqst = NULL;
    Rqst_arg *Current = NULL;

    while (Size > 0)
    {
        int chr = (int)(strchr(Request, UNIT_SCEPARATOR[0]) - Request);
        char *tmpRqst = calloc(chr, sizeof(char));
        strncpy(tmpRqst, Request, chr);
        tmpRqst[chr] = '\0';
        Rqst_arg *new = request_arg_gen(tmpRqst);
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
        memcpy(tmp, Request + chr + 1, Size -= chr - 1);
        free(Request);
        Request = tmp;
    }
    request_arg_insert(re, Top_rqst);
    return re;
}

Rqst *request_gen()
{
    Rqst *re = malloc(sizeof(Rqst));
    re->args = NULL;
    return re;
}

Rqst *request_gen_wUID(const char UID[16])
{
    Rqst *re = request_gen();
    memcpy(re->RespondToUID, UID, 16);

    return re;
}

void request_free(Rqst *Request)
{
    request_arg_free(Request->args);
    free(Request);
}

Rqst *request_gen_response(Rqst *Original, char OPCODE[4], Rqst_arg *Args)
{
    Rqst *re = request_gen();
    memcpy(re->UID, NULLREQUEST, 16);
    memcpy(re->RespondToUID, Original->UID, 16);
    memcpy(re->OPCODE, OPCODE, 4);
    re->args = Args;
    return re;
}

Rqst *request_gen_server_OPCODE(char OPCODE[4])
{
    return request_gen_response(request_gen_wUID(NULLREQUEST), OPCODE, NULL);
}

Rqst *request_gen_invalid(Rqst *Original)
{
    return request_gen_response(Original, _OPCODE_CLT_INVALID, NULL);
}

int request_get_nargs(Rqst *args)
{
    if (args == NULL)
        return 0;
    int nargs = 1;
    Rqst_arg *arg = args->args;
    while (arg->next == NULL)
    {
        nargs++;
        arg = arg->next;
    }
    return nargs;
}

Rqst_arg *request_arg_gen(const char *arg)
{
    Rqst_arg *Arg = calloc(1, sizeof(Rqst_arg));
    Arg->arg = calloc(strlen(arg) + 1, sizeof(char));
    strcpy(Arg->arg, arg);
    return Arg;
}

void request_arg_free(Rqst_arg *arg)
{
    if (arg == NULL)
        return;
    free(arg->arg);
    free(arg);
}

void request_args_free(Rqst_arg *arg)
{
    if (arg == NULL)
        return;
    Rqst_arg *current = arg;
    Rqst_arg *next;
    do
    {
        next = current->next;
        request_arg_free(current);
        current = next;
    } while (next != NULL);
    arg = NULL;
}

void request_arg_insert(Rqst *Request, Rqst_arg *arg)
{
    if (Request->args == NULL)
    {
        Request->args = arg;
    }
    else
    {
        Rqst_arg *last = Request->args;
        while (last->next != NULL)
            last = last->next;
        last->next = arg;
    }
}

void request_send_clt(Clt *Client, Rqst *Request)
{
    int nargs = request_get_nargs(Request);
    int len = strlen(Request->UID) + nargs * sizeof(char);
    for (Rqst_arg *arg = (Request->args); arg != NULL; arg = arg->next)
        len += strlen(arg->arg);
    char *content = calloc(len, sizeof(char));
    sprintf(content, "%s%s%s",
            Fbyte_FROM_int(len),
            Request->UID,
            UNIT_SCEPARATOR);
    int i = 0;
    for (Rqst_arg *arg = (Request->args); arg != NULL; arg = arg->next)
    {
        if (i == 0)
            i = !i;
        else
            strcat(content, UNIT_SCEPARATOR);
        strcat(content, arg->arg);
    }
    snd(Client, content, len);
}

void request_send_invalid(Clt *Client, Rqst *Original)
{
    request_send_clt(Client, request_gen_invalid(Original));
}
