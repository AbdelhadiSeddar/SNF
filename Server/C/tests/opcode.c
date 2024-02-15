#include <SRBNP/opcode.h>

void print_opcodes()
{
    printf("#\tListing Saaved OPcodes\n");
    SRBNP_opcode_LL_item *cat = SRBNP_opcode_LL;
    while (cat != NULL)
    {
        printf("Category Code [%x] : %s\n", cat->OPmmbr, cat->Definition);
        SRBNP_opcode_LL_item *subcat = cat->sub;
        while (subcat != NULL)
        {
            printf(" - Sub-Category Code [%x] : %s\n", subcat->OPmmbr, subcat->Definition);
            SRBNP_opcode_LL_item *cmd = subcat->sub;
            while (cmd != NULL)
            {
                printf("\t) Command Code [%x] : %s\n", cmd->OPmmbr, cmd->Definition);
                cmd = cmd->next;
            }
            subcat = subcat->next;
        }
        cat = cat->next;
    }
    
    printf("####### END\n");
}

int main()
{
    srbnp_opcode_init();
    if(!SRBNP_opcode_base_isinit)
        printf("Undefined\n");
    print_opcodes();

    srbnp_opcode_define_category(0x01, "Category Test");
    srbnp_opcode_define_sub_category(0x01, 0x00, "Sub Category 1");
    srbnp_opcode_define_sub_category(0x01, 0x01, "Sub Category 2");

    srbnp_opcode_define_command(0x01, 0x00, 0xAB, "Command Test 1-1");
    srbnp_opcode_define_command(0x01, 0x00, 0xCB, "Command Test 1-2");
    srbnp_opcode_define_command(0x01, 0x00, 0x4B, "Command Test 1-3");
    srbnp_opcode_define_command(0x01, 0x00, 0x5B, "Command Test 1-4");

    srbnp_opcode_define_command(0x01, 0x01, 0x67, "Command Test 2-1");
    srbnp_opcode_define_command(0x01, 0x01, 0xFE, "Command Test 2-2");
    srbnp_opcode_define_command(0x01, 0x01, 0x12, "Command Test 2-3");
    srbnp_opcode_define_command(0x01, 0x01, 0x3E, "Command Test 2-4");
    srbnp_opcode_define_command(0x01, 0x01, 0x6D, "Command Test 2-5");

    print_opcodes();
    return 0;
}