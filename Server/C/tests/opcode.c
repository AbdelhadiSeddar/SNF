#include <SNF.h>

void print_opcodes()
{
    printf("#\tListing Saaved OPcodes\n");
    SNF_opcode_LL_item *cat = SNF_opcode_LL;
    while (cat != NULL)
    {
        printf("Category Code [%x] : %s\n", cat->OPmmbr, cat->Definition);
        SNF_opcode_LL_item *subcat = cat->sub;
        while (subcat != NULL)
        {
            printf(" - Sub-Category Code [%x] : %s\n", subcat->OPmmbr, subcat->Definition);
            SNF_opcode_LL_item *cmd = subcat->sub;
            while (cmd != NULL)
            {
                printf("\t) Command Code [%x] : %s\n", cmd->OPmmbr, cmd->Definition);
                SNF_opcode_LL_item *det = cmd->sub;
                while (det != NULL)
                {
                    printf("\t  - Detail Code [%x] : %s\n", det->OPmmbr, det->Definition);
                    det = det->next;
                }
                cmd = cmd->next;
            }
            subcat = subcat->next;
        }
        cat = cat->next;
    }

    printf("####### END\n");
}

void print_detail(SNF_opcode_LL_item *op_ll)
{
    if (op_ll == NULL)
    {
        printf("Invalid's Detail wasnt found");
    }
    else
    {
        printf("Command [ %x ] : %s \n\tDetail [ %x ]: %s\n",
               op_ll->parent->OPmmbr,
               op_ll->parent->Definition,
               op_ll->OPmmbr,
               op_ll->Definition);
    }
}

int main()
{
    snf_opcode_init();
    if (!SNF_opcode_base_isinit)
        printf("Undefined\n");
    print_opcodes();

    snf_opcode_define_category(0x01, "Category Test");
    snf_opcode_define_sub_category(0x01, 0x00, "Sub Category 1");
    snf_opcode_define_sub_category(0x01, 0x01, "Sub Category 2");

    snf_opcode_define_command(0x01, 0x00, 0xAB, "Command Test 1-1", NULL);
    snf_opcode_define_command(0x01, 0x00, 0xCB, "Command Test 1-2", NULL);
    snf_opcode_define_command(0x01, 0x00, 0x4B, "Command Test 1-3", NULL);
    snf_opcode_define_command(0x01, 0x00, 0x5B, "Command Test 1-4", NULL);

    snf_opcode_define_command(0x01, 0x01, 0x67, "Command Test 2-1", NULL);
    snf_opcode_define_command(0x01, 0x01, 0xFE, "Command Test 2-2", NULL);
    snf_opcode_define_command(0x01, 0x01, 0x12, "Command Test 2-3", NULL);
    snf_opcode_define_command(0x01, 0x01, 0x3E, "Command Test 2-4", NULL);
    snf_opcode_define_command(0x01, 0x01, 0x6D, "Command Test 2-5", NULL);

    print_opcodes();

    SNF_opcode *op = snf_opcode_getu_base(SNF_OPCODE_BASE_CMD_INVALID);
    printf("Undetailed Invalid { Com: %x | Det: %x } \n", op->strct.Command, op->strct.Detail);
    free(op);
    op = snf_opcode_get_base(
            SNF_OPCODE_BASE_CMD_INVALID,
            SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL
        );
    printf("Undetailed Invalid { Com: %x | Det: %x } \n", op->strct.Command, op->strct.Detail);
    free(op);

    print_detail(snf_opcode_get_base_detail(
        SNF_OPCODE_BASE_CMD_INVALID,
        SNF_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE));
    print_detail(snf_opcode_get_base_detail(
        SNF_OPCODE_BASE_CMD_INVALID,
        SNF_OPCODE_BASE_DET_INVALID_UNIMPLEMENTED_OPCODE));

    return 0;
}