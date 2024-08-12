#include <SNF/opcode.h>
#include "opcode.h"

SNF_opcode_LL_item *SNF_opcode_LL = NULL;
int SNF_opcode_base_isinit = 0;

char *undetailed_definition = "Undefined Detail";

int snf_opcode_define_base()
{
    if (snf_opcode_define_category(
            SNF_OPCODE_BASE_CAT,
            "Base SNF Opcode category") < 0)
        return -1;
    if (snf_opcode_define_sub_category(
            SNF_OPCODE_BASE_CAT,
            SNF_OPCODE_BASE_SUBCAT,
            "Base SNF Opcode sub-category") < 0)
        return -1;

    if (snf_opcode_define_command(
            SNF_OPCODE_BASE_CAT,
            SNF_OPCODE_BASE_SUBCAT,
            SNF_OPCODE_BASE_CMD_CONNECT,
            "Used when client is attempting to connect",
            snf_cmd_invalid_error_protocol) < 0)
        return -1;
    if (snf_opcode_define_command(
            SNF_OPCODE_BASE_CAT,
            SNF_OPCODE_BASE_SUBCAT,
            SNF_OPCODE_BASE_CMD_RECONNECT,
            "Used when client is attempting to reconnect or is forced to.",
            snf_cmd_invalid_error_protocol) < 0)
        return -1;
    if (snf_opcode_define_command(
            SNF_OPCODE_BASE_CAT,
            SNF_OPCODE_BASE_SUBCAT,
            SNF_OPCODE_BASE_CMD_DISCONNECT,
            "Used when client is attempting to disconnect",
            snf_cmd_invalid_error_protocol) < 0)
        return -1;
    if (snf_opcode_define_command(
            SNF_OPCODE_BASE_CAT,
            SNF_OPCODE_BASE_SUBCAT,
            SNF_OPCODE_BASE_CMD_SNF_VER,
            "When client requests SNF version of the Server.",
            snf_cmd_snf_ver) < 0)
        return -1;
    if (snf_opcode_define_command(
            SNF_OPCODE_BASE_CAT,
            SNF_OPCODE_BASE_SUBCAT,
            SNF_OPCODE_BASE_CMD_KICK,
            "Used when client is Kicked",
            snf_cmd_invalid_error_protocol) < 0)
        return -1;
    if (snf_opcode_define_command(
            SNF_OPCODE_BASE_CAT,
            SNF_OPCODE_BASE_SUBCAT,
            SNF_OPCODE_BASE_CMD_CONFIRM,
            "Used when client's request was confirmed",
            snf_cmd_invalid_error_protocol) < 0)
        return -1;
    if (snf_opcode_define_command(
            SNF_OPCODE_BASE_CAT,
            SNF_OPCODE_BASE_SUBCAT,
            SNF_OPCODE_BASE_CMD_REJECT,
            "Used when client's request was rejected",
            snf_cmd_invalid_error_protocol) < 0)
        return -1;
    if (snf_opcode_define_command(
            SNF_OPCODE_BASE_CAT,
            SNF_OPCODE_BASE_SUBCAT,
            SNF_OPCODE_BASE_CMD_INVALID,
            "Used when client's request was invalid",
            snf_cmd_invalid_error_protocol) < 0)
        return -1;

    if (snf_opcode_define_detail(
            SNF_OPCODE_BASE_CAT,
            SNF_OPCODE_BASE_SUBCAT,
            SNF_OPCODE_BASE_CMD_INVALID,
            SNF_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE,
            "Used when Received opcode was not registred") < 0)
        return -1;
    if (snf_opcode_define_detail(
            SNF_OPCODE_BASE_CAT,
            SNF_OPCODE_BASE_SUBCAT,
            SNF_OPCODE_BASE_CMD_INVALID,
            SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL,
            "Used when Protocol used is invalid") < 0)
        return -1;
    if (snf_opcode_define_detail(
            SNF_OPCODE_BASE_CAT,
            SNF_OPCODE_BASE_SUBCAT,
            SNF_OPCODE_BASE_CMD_INVALID,
            SNF_OPCODE_BASE_DET_INVALID_UNIMPLEMENTED_OPCODE,
            "Received opcode does not have a function to call") < 0)
        return -1;
    SNF_opcode_base_isinit = 1;
    return 0;
}

int snf_opcode_init()
{
    if(SNF_opcode_base_isinit)
        return 0;
    return snf_opcode_define_base();
}

int snf_opcode_define_category(
    SNF_opcode_mmbr_t Code,
    const char *Definition)
{
    SNF_opcode_LL_item *item = calloc(1, sizeof(SNF_opcode_LL_item));
    if (item == NULL)
        return -1;

    item->OPmmbr = Code;
    item->Definition = calloc(strlen(Definition) + 1, sizeof(char));

    if (item->Definition == NULL)
        return -1;

    strcpy(item->Definition, Definition);

    SNF_opcode_LL_item *re = SNF_opcode_LL;
    if (re == NULL)
        SNF_opcode_LL = item;
    else
        while (re != NULL)
        {
            if (re->next == NULL)
            {
                re->next = item;
                break;
            }
            else if (re->next->OPmmbr == item->OPmmbr)
                return 1;
            re = re->next;
        }

    return 0;
}

int snf_opcode_define_sub_category(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t Code,
    const char *Definition)
{
    SNF_opcode_LL_item *item = calloc(1, sizeof(SNF_opcode_LL_item));
    if (item == NULL)
        return -1;

    item->OPmmbr = Code;
    item->Definition = calloc(strlen(Definition) + 1, sizeof(char));

    if (item->Definition == NULL)
        return -1;

    strcpy(item->Definition, Definition);

    if (SNF_opcode_LL == NULL)
        return -2;
    SNF_opcode_LL_item *sub = snf_opcode_get_category(Category);
    if (sub == NULL)
        return -2;

    item->parent = sub;
    if (sub->sub == NULL)
        sub->sub = item;
    else
    {
        sub = sub->sub;
        while (sub != NULL)
        {
            if (sub->next == NULL)
            {
                sub->next = item;
                break;
            }
            else if (sub->next->OPmmbr == item->OPmmbr)
                return 1;
            sub = sub->next;
        }
    }
    return 0;
}
int snf_opcode_define_command(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Code,
    const char *Definition,
    SNF_RQST *(func)(SNF_RQST *))
{

    SNF_opcode_LL_item *item = calloc(1, sizeof(SNF_opcode_LL_item));
    if (item == NULL)
        return -1;

    item->OPmmbr = Code;
    item->Definition = calloc(strlen(Definition) + 1, sizeof(char));
    item->sub = calloc(1, sizeof(SNF_opcode_LL_item));
    item->sub->OPmmbr = SNF_OPCODE_BASE_DET_UNDETAILED;
    item->sub->Definition = undetailed_definition;
    item->func = func;
    item->sub->next = (item->sub->sub = NULL);
    item->sub->parent = item;

    if (item->Definition == NULL)
        return -1;

    strcpy(item->Definition, Definition);

    if (SNF_opcode_LL == NULL)
        return -2;
    SNF_opcode_LL_item *sub = snf_opcode_get_sub_category(Category, SubCategory);
    if (sub == NULL)
        return -2;
    item->parent = sub;
    if (sub->sub == NULL)
        sub->sub = item;
    else
    {
        sub = sub->sub;
        while (sub != NULL)
        {
            if (sub->next == NULL)
            {
                sub->next = item;
                break;
            }
            else if (sub->next->OPmmbr == item->OPmmbr)
                return 1;
            sub = sub->next;
        }
    }
    return 0;
}
int snf_opcode_define_detail(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Code,
    const char *Definition)
{

    SNF_opcode_LL_item *item = calloc(1, sizeof(SNF_opcode_LL_item));
    if (item == NULL)
        return -1;

    item->OPmmbr = Code;
    item->Definition = calloc(strlen(Definition) + 1, sizeof(char));

    if (item->Definition == NULL)
        return -1;

    strcpy(item->Definition, Definition);

    if (SNF_opcode_LL == NULL)
        return -2;
    SNF_opcode_LL_item *sub = snf_opcode_get_command(Category, SubCategory, Command);
    if (sub == NULL)
        return -2;
    item->parent = sub;
    if (sub->sub == NULL)
        sub->sub = item;
    else
    {
        sub = sub->sub;
        while (sub != NULL)
        {
            if (sub->next == NULL)
            {
                sub->next = item;
                break;
            }
            else if (sub->next->OPmmbr == item->OPmmbr)
                return 1;
            sub = sub->next;
        }
    }
    return 0;
}
SNF_opcode_LL_item *snf_opcode_get_category(
    SNF_opcode_mmbr_t Category)
{
    SNF_opcode_LL_item *srch = SNF_opcode_LL;

    while (srch != NULL)
        if (srch->OPmmbr == Category)
            break;
        else
            srch = srch->next;

    return srch;
}
SNF_opcode_LL_item *snf_opcode_get_sub_category(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory)
{
    SNF_opcode_LL_item *srch = snf_opcode_get_category(Category);
    if (srch == NULL)
        return NULL;
    srch = srch->sub;

    while (srch != NULL)
        if (srch->OPmmbr == SubCategory)
            break;
        else
            srch = srch->next;
    return srch;
}
SNF_opcode_LL_item *snf_opcode_get_command(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command)
{
    SNF_opcode_LL_item *srch = snf_opcode_get_sub_category(Category, SubCategory);
    if (srch == NULL)
        return NULL;
    srch = srch->sub;

    while (srch != NULL)
        if (srch->OPmmbr == Command)
            break;
        else
            srch = srch->next;
    return srch;
}
SNF_opcode_LL_item *snf_opcode_get_detail(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Detail)
{
    SNF_opcode_LL_item *srch = snf_opcode_get_command(Category, SubCategory, Command);
    if (srch == NULL)
        return NULL;
    srch = srch->sub;

    while (srch != NULL)
        if (srch->OPmmbr == Detail)
            break;
        else
            srch = srch->next;

    return srch;
}

#define _SNF_OPCODE_GET_INVALID                      \
    re->strct.Category = SNF_OPCODE_BASE_CAT;        \
    re->strct.SubCategory = SNF_OPCODE_BASE_SUBCAT;  \
    re->strct.Command = SNF_OPCODE_BASE_CMD_INVALID; \
    re->strct.Detail = SNF_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE;
SNF_opcode *snf_opcode_get(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Detail)
{
    if (!SNF_opcode_base_isinit)
    {
        perror("Base Opcode has not been initialized!");
        exit(EXIT_FAILURE);
    }
    SNF_opcode_LL_item *item = snf_opcode_get_detail(
        Category,
        SubCategory,
        Command,
        Detail);

    SNF_opcode *re = calloc(1, sizeof(SNF_opcode));
    if (item == NULL)
    {
        _SNF_OPCODE_GET_INVALID
    }
    else
    {
        re->strct.Category = Category;
        re->strct.SubCategory = SubCategory;
        re->strct.Command = Command;
        re->strct.Detail = Detail;
    }
    return re;
}
SNF_opcode *snf_opcode_getu(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command)
{
    return snf_opcode_get(
        Category,
        SubCategory,
        Command,
        SNF_OPCODE_BASE_DET_UNDETAILED);

    if (!SNF_opcode_base_isinit)
    {
        perror("Base Opcode has not been initialized!");
        exit(EXIT_FAILURE);
    }
    SNF_opcode_LL_item *item = snf_opcode_get_command(
        Category,
        SubCategory,
        Command);

    SNF_opcode *re = calloc(1, sizeof(SNF_opcode));
    if (item == NULL)
    {
        _SNF_OPCODE_GET_INVALID
    }
    else
    {
        re->strct.Category = Category;
        re->strct.SubCategory = SubCategory;
        re->strct.Command = Command;
        re->strct.Detail = item->sub->OPmmbr;
    }
    return re;
}

int snf_opcode_compare(SNF_opcode *op1, SNF_opcode *op2)
{
    if (!SNF_opcode_base_isinit)
    {
        perror("Base Opcode has not been initialized!");
        exit(EXIT_FAILURE);
    }
    if (op1 == NULL || op2 == NULL)
        return -1;

    for (int i = 0; i < 3; i++)
    {
        if (op1->opcode[i] != op2->opcode[i])
            return -2;
    }
    int chk;
    if (!(chk = op1->strct.Detail - op2->strct.Detail))
        return 0;
    else if (chk > 0)
        return 1;
    else
        return 2;
}

int snf_opcode_isbase(SNF_opcode *op)
{
    if(op->strct.Category != SNF_OPCODE_BASE_CAT)
        return 0;
    if(op->strct.SubCategory != SNF_OPCODE_BASE_SUBCAT)
        return 0;
    
    if((op->strct.Command < SNF_OPCODE_BASE_CMD_CONNECT
        || op->strct.Command > SNF_OPCODE_BASE_CMD_REJECT )
        && op->strct.Detail != SNF_OPCODE_BASE_CMD_INVALID
    )
    return 0;

    return 1;
}
SNF_opcode_LL_item *snf_opcode_get_base_category()
{
    return snf_opcode_get_category(
        SNF_OPCODE_BASE_CAT);
}

SNF_opcode_LL_item *snf_opcode_get_base_sub_category()
{
    return snf_opcode_get_sub_category(
        SNF_OPCODE_BASE_CAT,
        SNF_OPCODE_BASE_SUBCAT);
}

SNF_opcode_LL_item *snf_opcode_get_base_command(
    SNF_opcode_mmbr_t Command)
{
    return snf_opcode_get_command(
        SNF_OPCODE_BASE_CAT,
        SNF_OPCODE_BASE_SUBCAT,
        Command);
}

SNF_opcode_LL_item *snf_opcode_get_base_detail(
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Detail)
{
    return snf_opcode_get_detail(
        SNF_OPCODE_BASE_CAT,
        SNF_OPCODE_BASE_SUBCAT,
        Command,
        Detail);
}

SNF_opcode *snf_opcode_get_base(
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Detail)
{
    return snf_opcode_get(
        SNF_OPCODE_BASE_CAT,
        SNF_OPCODE_BASE_SUBCAT,
        Command,
        Detail);
}

SNF_opcode *snf_opcode_getu_base(SNF_opcode_mmbr_t Command)
{
    return snf_opcode_get_base(
        Command,
        SNF_OPCODE_BASE_DET_UNDETAILED);
}
