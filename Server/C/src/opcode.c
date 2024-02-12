#include <SRBNP/_BASE_OPCODE.h>
#include <SRBNP/opcode.h>

SRBNP_opcode_membr_LL_item *SRBNP_opcode_LL = NULL;
int SRBNP_opcode_base_isinit = 0;
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
            "Used when client is attempting to reconnect or is commanded to.") < 0)
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
            "Used when client is requesting Server version") < 0)
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
    SRBNP_opcode_base_isinit = 1;
    return 0;
}

int srbnp_opcode_init()
{
    return srbnp_opcode_base_init();
}

int srbnp_opcode_define_category(
    SRBNP_opcode_mmber_t Code,
    const char *Definition)
{
    SRBNP_opcode_membr_LL_item *item = calloc(1, sizeof(SRBNP_opcode_membr_LL_item));
    if (item == NULL)
        return -1;

    item->OPmmbr = Code;
    char *def = calloc(strnlen(Definition, 1), sizeof(char));

    if (def == NULL)
        return NULL;
    if (def[0] != 0)
        strcpy(def, Definition);

    SRBNP_opcode_membr_LL_item *re = SRBNP_opcode_LL;
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
    SRBNP_opcode_mmber_t Category,
    SRBNP_opcode_mmber_t Code,
    const char *Definition)
{
    SRBNP_opcode_membr_LL_item *item = calloc(1, sizeof(SRBNP_opcode_membr_LL_item));
    if (item == NULL)
        return -1;

    item->OPmmbr = Code;
    char *def = calloc(strnlen(Definition, 1), sizeof(char));

    if (def == NULL)
        return NULL;
    if (def[0] != 0)
        strcpy(def, Definition);

    if (SRBNP_opcode_LL == NULL)
        return -1;
    SRBNP_opcode_membr_LL_item *sub = srbnp_opcode_get_category(Category);
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
                SRBNP_opcode_LL->next = item;
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
    SRBNP_opcode_mmber_t Category,
    SRBNP_opcode_mmber_t SubCategory,
    SRBNP_opcode_mmber_t Code,
    const char *Definition)
{

    SRBNP_opcode_membr_LL_item *item = calloc(1, sizeof(SRBNP_opcode_membr_LL_item));
    if (item == NULL)
        return -1;

    item->OPmmbr = Code;
    char *def = calloc(strnlen(Definition, 1), sizeof(char));

    if (def == NULL)
        return NULL;
    if (def[0] != 0)
        strcpy(def, Definition);

    if (SRBNP_opcode_LL == NULL)
        return -1;
    SRBNP_opcode_membr_LL_item *sub = srbnp_opcode_get_sub_category(Category, SubCategory);
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
                SRBNP_opcode_LL->next = item;
                break;
            }
            else if (sub->next->OPmmbr == item->OPmmbr)
                return 1;
            sub = sub->next;
        }
    }
    return 0;
}
SRBNP_opcode_membr_LL_item srbnp_opcode_get_category(
    SRBNP_opcode_mmber_t Category)
{
    SRBNP_opcode_membr_LL_item *srch = SRBNP_opcode_LL;

    while (srch != NULL)
        if (srch->OPmmbr == Category)
            break;
        else
            srch = srch->next;

    return srch;
}
SRBNP_opcode_membr_LL_item srbnp_opcode_get_sub_category(
    SRBNP_opcode_mmber_t Category,
    SRBNP_opcode_mmber_t SubCategory)
{
    SRBNP_opcode_membr_LL_item *srch = srbnp_opcode_get_category(Category);
    if (srch == NULL)
        return NULL;
    srch = such->sub;

    while (srch != NULL)
        if (srch->OPmmbr == Category)
            break;
        else
            srch = srch->next;
    return srch;
}
SRBNP_opcode_membr_LL_item srbnp_opcode_get_command(
    SRBNP_opcode_mmber_t Category,
    SRBNP_opcode_mmber_t SubCategory,
    SRBNP_opcode_mmber_t Command)
{
    SRBNP_opcode_membr_LL_item *srch = srbnp_opcode_get_sub_category(Category, SubCategory);
    if (srch == NULL)
        return NULL;
    srch = such->sub;

    while (srch != NULL)
        if (srch->OPmmbr == Category)
            break;
        else
            srch = srch->next;
    return srch;
}
SRBNP_opcode srbnp_opcode_get(
    SRBNP_opcode_mmber_t Category,
    SRBNP_opcode_mmber_t SubCategory,
    SRBNP_opcode_mmber_t Command,
    SRBNP_opcode_mmber_t Detail)
{
    if (!srbnp_opcode_base_isinit)
    {
        perror("Base Opcode has not been initialized!");
        exit(EXIT_FAILURE);
    }
    SRBNP_opcode_membr_LL_item *item = srbnp_opcode_get_command(
        Category,
        SubCategory,
        Command);

    if (item == NULL)
        return srbnp_opcode_get(
            SRBNP_OPCODE_BASE_CAT,
            SRBNP_OPCODE_BASE_SUBCAT,
            SRBNP_OPCODE_BASE_CMD_INVALID,
            SRBNP_OPCODE_DET_INVALID_UNREGISTRED_OPCODE);

    SRBNP_opcode re;
    re.strct.Category = Category;
    re.strct.SubCategory = SubCategory;
    re.strct.Command = Command;
    re.strct.Detail = Detail;

    return re;
}

int main()
{
    SRBNP_opcode test;

    return 0;
}