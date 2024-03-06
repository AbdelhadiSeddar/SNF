#include <SRBNP/opcode.h>

SRBNP_opcode_LL_item *SRBNP_opcode_LL = NULL;
int SRBNP_opcode_base_isinit = 0;

char *undetailed_definition = "Undefined Detail";

int srbnp_opcode_define_base()
{
    if (srbnp_opcode_define_category(
            SRBNP_OPCODE_BASE_CAT,
            "Base SRBNP Opcode category") < 0)
        return -1;
    if (srbnp_opcode_define_sub_category(
            SRBNP_OPCODE_BASE_CAT,
            SRBNP_OPCODE_BASE_SUBCAT,
            "Base SRBNP Opcode sub-category") < 0)
        return -1;

    if (srbnp_opcode_define_command(
            SRBNP_OPCODE_BASE_CAT,
            SRBNP_OPCODE_BASE_SUBCAT,
            SRBNP_OPCODE_BASE_CMD_CONNECT,
            "Used when client is attempting to connect") < 0)
        return -1;
    if (srbnp_opcode_define_command(
            SRBNP_OPCODE_BASE_CAT,
            SRBNP_OPCODE_BASE_SUBCAT,
            SRBNP_OPCODE_BASE_CMD_RECONNECT,
            "Used when client is attempting to reconnect or is forced to.") < 0)
        return -1;
    if (srbnp_opcode_define_command(
            SRBNP_OPCODE_BASE_CAT,
            SRBNP_OPCODE_BASE_SUBCAT,
            SRBNP_OPCODE_BASE_CMD_DISCONNECT,
            "Used when client is attempting to disconnect") < 0)
        return -1;
    if (srbnp_opcode_define_command(
            SRBNP_OPCODE_BASE_CAT,
            SRBNP_OPCODE_BASE_SUBCAT,
            SRBNP_OPCODE_BASE_CMD_SRBNP_VER,
            "When client requests SRBNP version of the Server.") < 0)
        return -1;
    if (srbnp_opcode_define_command(
            SRBNP_OPCODE_BASE_CAT,
            SRBNP_OPCODE_BASE_SUBCAT,
            SRBNP_OPCODE_BASE_CMD_KICK,
            "Used when client is Kicked") < 0)
        return -1;
    if (srbnp_opcode_define_command(
            SRBNP_OPCODE_BASE_CAT,
            SRBNP_OPCODE_BASE_SUBCAT,
            SRBNP_OPCODE_BASE_CMD_CONFIRM,
            "Used when client's request was confirmed") < 0)
        return -1;
    if (srbnp_opcode_define_command(
            SRBNP_OPCODE_BASE_CAT,
            SRBNP_OPCODE_BASE_SUBCAT,
            SRBNP_OPCODE_BASE_CMD_REJECT,
            "Used when client's request was rejected") < 0)
        return -1;
    if (srbnp_opcode_define_command(
            SRBNP_OPCODE_BASE_CAT,
            SRBNP_OPCODE_BASE_SUBCAT,
            SRBNP_OPCODE_BASE_CMD_INVALID,
            "Used when client's request was invalid") < 0)
        return -1;

    if (srbnp_opcode_define_detail(
            SRBNP_OPCODE_BASE_CAT,
            SRBNP_OPCODE_BASE_SUBCAT,
            SRBNP_OPCODE_BASE_CMD_INVALID,
            SRBNP_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE,
            "Used when Received opcode was not registred") < 0)
        return -1;
    if (srbnp_opcode_define_detail(
            SRBNP_OPCODE_BASE_CAT,
            SRBNP_OPCODE_BASE_SUBCAT,
            SRBNP_OPCODE_BASE_CMD_INVALID,
            SRBNP_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL,
            "Used when Protocol used is invalid") < 0)
        return -1;
    SRBNP_opcode_base_isinit = 1;
    return 0;
}

int srbnp_opcode_init()
{
    return srbnp_opcode_define_base();
}

int srbnp_opcode_define_category(
    SRBNP_opcode_mmbr_t Code,
    const char *Definition)
{
    SRBNP_opcode_LL_item *item = calloc(1, sizeof(SRBNP_opcode_LL_item));
    if (item == NULL)
        return -1;

    item->OPmmbr = Code;
    item->Definition = calloc(strlen(Definition) + 1, sizeof(char));

    if (item->Definition == NULL)
        return -1;

    strcpy(item->Definition, Definition);

    SRBNP_opcode_LL_item *re = SRBNP_opcode_LL;
    if (re == NULL)
        SRBNP_opcode_LL = item;
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

int srbnp_opcode_define_sub_category(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t Code,
    const char *Definition)
{
    SRBNP_opcode_LL_item *item = calloc(1, sizeof(SRBNP_opcode_LL_item));
    if (item == NULL)
        return -1;

    item->OPmmbr = Code;
    item->Definition = calloc(strlen(Definition) + 1, sizeof(char));

    if (item->Definition == NULL)
        return -1;

    strcpy(item->Definition, Definition);

    if (SRBNP_opcode_LL == NULL)
        return -2;
    SRBNP_opcode_LL_item *sub = srbnp_opcode_get_category(Category);
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
int srbnp_opcode_define_command(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Code,
    const char *Definition)
{

    SRBNP_opcode_LL_item *item = calloc(1, sizeof(SRBNP_opcode_LL_item));
    if (item == NULL)
        return -1;

    item->OPmmbr = Code;
    item->Definition = calloc(strlen(Definition) + 1, sizeof(char));
    item->sub = calloc(1, sizeof(SRBNP_opcode_LL_item));
    item->sub->OPmmbr = SRBNP_OPCODE_BASE_DET_UNDETAILED;
    item->sub->Definition = undetailed_definition;
    item->sub->next = (item->sub->sub = NULL);
    item->sub->parent = item;

    if (item->Definition == NULL)
        return -1;

    strcpy(item->Definition, Definition);

    if (SRBNP_opcode_LL == NULL)
        return -2;
    SRBNP_opcode_LL_item *sub = srbnp_opcode_get_sub_category(Category, SubCategory);
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
int srbnp_opcode_define_detail(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Command,
    SRBNP_opcode_mmbr_t Code,
    const char *Definition)
{

    SRBNP_opcode_LL_item *item = calloc(1, sizeof(SRBNP_opcode_LL_item));
    if (item == NULL)
        return -1;

    item->OPmmbr = Code;
    item->Definition = calloc(strlen(Definition) + 1, sizeof(char));

    if (item->Definition == NULL)
        return -1;

    strcpy(item->Definition, Definition);

    if (SRBNP_opcode_LL == NULL)
        return -2;
    SRBNP_opcode_LL_item *sub = srbnp_opcode_get_command(Category, SubCategory, Command);
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
SRBNP_opcode_LL_item *srbnp_opcode_get_category(
    SRBNP_opcode_mmbr_t Category)
{
    SRBNP_opcode_LL_item *srch = SRBNP_opcode_LL;

    while (srch != NULL)
        if (srch->OPmmbr == Category)
            break;
        else
            srch = srch->next;

    return srch;
}
SRBNP_opcode_LL_item *srbnp_opcode_get_sub_category(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory)
{
    SRBNP_opcode_LL_item *srch = srbnp_opcode_get_category(Category);
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
SRBNP_opcode_LL_item *srbnp_opcode_get_command(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Command)
{
    SRBNP_opcode_LL_item *srch = srbnp_opcode_get_sub_category(Category, SubCategory);
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
SRBNP_opcode_LL_item *srbnp_opcode_get_detail(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Command,
    SRBNP_opcode_mmbr_t Detail)
{
    SRBNP_opcode_LL_item *srch = srbnp_opcode_get_command(Category, SubCategory, Command);
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

#define _SRBNP_OPCODE_GET_INVALID                      \
    re->strct.Category = SRBNP_OPCODE_BASE_CAT;        \
    re->strct.SubCategory = SRBNP_OPCODE_BASE_SUBCAT;  \
    re->strct.Command = SRBNP_OPCODE_BASE_CMD_INVALID; \
    re->strct.Detail = SRBNP_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE;
SRBNP_opcode *srbnp_opcode_get(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Command,
    SRBNP_opcode_mmbr_t Detail)
{
    if (!SRBNP_opcode_base_isinit)
    {
        perror("Base Opcode has not been initialized!");
        exit(EXIT_FAILURE);
    }
    SRBNP_opcode_LL_item *item = srbnp_opcode_get_detail(
        Category,
        SubCategory,
        Command,
        Detail);

    SRBNP_opcode *re = calloc(1, sizeof(SRBNP_opcode));
    if (item == NULL)
    {
        _SRBNP_OPCODE_GET_INVALID
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
SRBNP_opcode *srbnp_opcode_getu(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Command)
{
    return srbnp_opcode_get(
        Category,
        SubCategory,
        Command,
        SRBNP_OPCODE_BASE_DET_UNDETAILED);

    if (!SRBNP_opcode_base_isinit)
    {
        perror("Base Opcode has not been initialized!");
        exit(EXIT_FAILURE);
    }
    SRBNP_opcode_LL_item *item = srbnp_opcode_get_command(
        Category,
        SubCategory,
        Command);

    SRBNP_opcode *re = calloc(1, sizeof(SRBNP_opcode));
    if (item == NULL)
    {
        _SRBNP_OPCODE_GET_INVALID
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

int srbnp_opcode_compare(SRBNP_opcode *op1, SRBNP_opcode *op2)
{
    if (!SRBNP_opcode_base_isinit)
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

SRBNP_opcode_LL_item *srbnp_opcode_get_base_category()
{
    return srbnp_opcode_get_category(
        SRBNP_OPCODE_BASE_CAT);
}

SRBNP_opcode_LL_item *srbnp_opcode_get_base_sub_category()
{
    return srbnp_opcode_get_sub_category(
        SRBNP_OPCODE_BASE_CAT,
        SRBNP_OPCODE_BASE_SUBCAT);
}

SRBNP_opcode_LL_item *srbnp_opcode_get_base_command(
    SRBNP_opcode_mmbr_t Command)
{
    return srbnp_opcode_get_command(
        SRBNP_OPCODE_BASE_CAT,
        SRBNP_OPCODE_BASE_SUBCAT,
        Command);
}

SRBNP_opcode_LL_item *srbnp_opcode_get_base_detail(
    SRBNP_opcode_mmbr_t Command,
    SRBNP_opcode_mmbr_t Detail)
{
    return srbnp_opcode_get_detail(
        SRBNP_OPCODE_BASE_CAT,
        SRBNP_OPCODE_BASE_SUBCAT,
        Command,
        Detail);
}

SRBNP_opcode *srbnp_opcode_get_base(
    SRBNP_opcode_mmbr_t Command,
    SRBNP_opcode_mmbr_t Detail)
{
    return srbnp_opcode_get(
        SRBNP_OPCODE_BASE_CAT,
        SRBNP_OPCODE_BASE_SUBCAT,
        Command,
        Detail);
}

SRBNP_opcode *srbnp_opcode_getu_base(SRBNP_opcode_mmbr_t Command)
{
    return srbnp_opcode_get_base(
        Command,
        SRBNP_OPCODE_BASE_DET_UNDETAILED);
}
