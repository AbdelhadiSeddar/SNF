#ifndef opcode_h
#define opcode_h

#include <SRBNP/SRBNP.h>
#define SRBNP_opcode_mmbr_t uint8_t
#define SRBNP_opcode_mmbr_MAX UINT8_MAX

typedef union SRBNP_opcode_t SRBNP_opcode;
typedef struct SRBNP_opcode_LL_item_t SRBNP_opcode_LL_item;

union SRBNP_opcode_t
{
    /// @brief SRBNP_opcode's Structure
    struct SRBNP_opcode_struct
    {
        SRBNP_opcode_mmbr_t Category;
        SRBNP_opcode_mmbr_t SubCategory;
        SRBNP_opcode_mmbr_t Command;
        SRBNP_opcode_mmbr_t Detail;
    } *strct;
    SRBNP_opcode_mmbr_t opcode[4];
};
struct SRBNP_opcode_LL_item_t
{
    SRBNP_opcode_mmbr_t OPmmbr;
    char *Definition;
    SRBNP_opcode_LL_item *next;
    SRBNP_opcode_LL_item *parent;
    SRBNP_opcode_LL_item *sub;
};

extern SRBNP_opcode_LL_item *SRBNP_opcode_LL;
extern int SRBNP_opcode_base_isinit;
extern int srbnp_opcode_init();

extern int srbnp_opcode_define_category(
    SRBNP_opcode_mmbr_t Code,
    const char *Definition);
extern int srbnp_opcode_define_sub_category(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t Code,
    const char *Definition);
extern int srbnp_opcode_define_command(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Code,
    const char *Definition);

extern SRBNP_opcode_LL_item *srbnp_opcode_get_category(
    SRBNP_opcode_mmbr_t Category);
extern SRBNP_opcode_LL_item *srbnp_opcode_get_sub_category(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory);
extern SRBNP_opcode_LL_item *srbnp_opcode_get_command(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Command);
extern SRBNP_opcode *srbnp_opcode_get(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Command,
    SRBNP_opcode_mmbr_t Detail);

extern int srbnp_opcode_compare(
    SRBNP_opcode *op1,
    SRBNP_opcode *op2);

#pragma region[Base Opcode Values]
#define SRBNP_OPCODE_BASE_CAT (SRBNP_opcode_mmbr_t)0x00
#define SRBNP_OPCODE_BASE_SUBCAT (SRBNP_opcode_mmbr_t)0x00
#define SRBNP_OPCODE_BASE_DET_UNDETAILED (SRBNP_opcode_mmbr_t)0x00

#define SRBNP_OPCODE_BASE_CMD_CONNECT (SRBNP_opcode_mmbr_t)0x00
#define SRBNP_OPCODE_BASE_CMD_RECONNECT (SRBNP_opcode_mmbr_t)0x01
#define SRBNP_OPCODE_BASE_CMD_DISCONNECT (SRBNP_opcode_mmbr_t)0x02
#define SRBNP_OPCODE_BASE_CMD_SRBNP_VER (SRBNP_opcode_mmbr_t)0x03
#define SRBNP_OPCODE_BASE_CMD_KICK (SRBNP_opcode_mmbr_t)0x04

#define SRBNP_OPCODE_BASE_CMD_CONFIRM (SRBNP_opcode_mmbr_t)0x05
#define SRBNP_OPCODE_BASE_CMD_REJECT (SRBNP_opcode_mmbr_t)0x06

#pragma region[Base Command: Invalid]
#define SRBNP_OPCODE_BASE_CMD_INVALID (SRBNP_opcode_mmbr_t)0xFF
#define SRBNP_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE (SRBNP_opcode_mmbr_t)0x01
#define SRBNP_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL (SRBNP_opcode_mmbr_t)0x02
#pragma endregion
#pragma endregion

#pragma region[Base Opcode Function]
extern SRBNP_opcode_LL_item *srbnp_opcode_get_base_category();
extern SRBNP_opcode_LL_item *srbnp_opcode_get_base_sub_category();
extern SRBNP_opcode_LL_item *srbnp_opcode_get_base_command(
    SRBNP_opcode_mmbr_t Command);
extern SRBNP_opcode *srbnp_opcode_get_base(
    SRBNP_opcode_mmbr_t Command,
    SRBNP_opcode_mmbr_t Detail);
extern SRBNP_opcode *srbnp_opcode_get_invalid(
    SRBNP_opcode_mmbr_t Detail);
#pragma endregion

#endif