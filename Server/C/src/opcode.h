#ifndef opcode_h
#define opcode_h

#include <SRBNP/SRBNP.h>

#define SRBNP_opcode_mmber_t uint8_t
#define SRBNP_opcode_mmber_MAX UINT8_MAX

typedef union SRBNP_opcode_t SRBNP_opcode;
typedef struct SRBNP_opcode_LL_item_t SRBNP_opcode_LL_item;

union SRBNP_opcode_t
{
    /// @brief SRBNP_opcode's Structure
    struct SRBNP_opcode_struct
    {
        SRBNP_opcode_mmber_t Category;
        SRBNP_opcode_mmber_t SubCategory;
        SRBNP_opcode_mmber_t Command;
        SRBNP_opcode_mmber_t Detail;
    } strct;
    SRBNP_opcode_mmber_t opcode[4];
};
struct SRBNP_opcode_LL_item_t
{
    SRBNP_opcode_mmber_t OPmmbr;
    char *Definition;
    SRBNP_opcode_LL_item *next;
    SRBNP_opcode_LL_item *parent;
    SRBNP_opcode_LL_item *sub;
};

extern int srbnp_opcode_base_init();

extern int srbnp_opcode_define_category(
    SRBNP_opcode_mmber_t Code,
    const char *Definition);
extern int srbnp_opcode_define_sub_category(
    SRBNP_opcode_mmber_t Category,
    SRBNP_opcode_mmber_t Code,
    const char *Definition);
extern int srbnp_opcode_define_command(
    SRBNP_opcode_mmber_t Category,
    SRBNP_opcode_mmber_t SubCategory,
    SRBNP_opcode_mmber_t Code,
    const char *Definition);

extern SRBNP_opcode_LL_item srbnp_opcode_get_category(
    SRBNP_opcode_mmber_t Category);
extern SRBNP_opcode_LL_item srbnp_opcode_get_sub_category(
    SRBNP_opcode_mmber_t Category,
    SRBNP_opcode_mmber_t SubCategory);
extern SRBNP_opcode_LL_item srbnp_opcode_get_command(
    SRBNP_opcode_mmber_t Category,
    SRBNP_opcode_mmber_t SubCategory,
    SRBNP_opcode_mmber_t Command);
extern SRBNP_opcode srbnp_opcode_get(
    SRBNP_opcode_mmber_t Category,
    SRBNP_opcode_mmber_t SubCategory,
    SRBNP_opcode_mmber_t Command,
    SRBNP_opcode_mmber_t Detail);

#endif