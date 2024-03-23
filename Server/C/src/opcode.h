#ifndef opcode_h
#define opcode_h

#include <SNF/SNF.h>

#define SNF_opcode_mmbr_t uint8_t
#define SNF_opcode_mmbr_MAX UINT8_MAX

/// @brief Structure for SNF's opcodes.
typedef union SNF_opcode_t SNF_opcode;
/// @brief Structure for saving SNF's categories, sub-categories, commands
typedef struct SNF_opcode_LL_item_t SNF_opcode_LL_item;

union SNF_opcode_t
{
    /// @brief SNF_opcode's Structure
    struct SNF_opcode_struct
    {
        SNF_opcode_mmbr_t Category;
        SNF_opcode_mmbr_t SubCategory;
        SNF_opcode_mmbr_t Command;
        SNF_opcode_mmbr_t Detail;
    } strct;
    SNF_opcode_mmbr_t opcode[4];
};
struct SNF_opcode_LL_item_t
{
    SNF_opcode_mmbr_t OPmmbr;
    char *Definition;
    SNF_opcode_LL_item *next;
    SNF_opcode_LL_item *parent;
    SNF_opcode_LL_item *sub;
};

/// @brief Data structure where opcodes will be saved.
extern SNF_opcode_LL_item *SNF_opcode_LL;
/// @brief used to check if SNF's base opcodes are initialized
extern int SNF_opcode_base_isinit;
/// @brief Initializes the SNF's opcodes
/// @return 0 On Success || -1 On fail { Shall fail if calloc fails, check calloc error codes }
extern int snf_opcode_init();
/// @brief Defines an opcode category
/// @param Code OPcode category's value
/// @param Definition String Definition for The category
/// @return 0 On Success || 1 if Category Already Exists || -1 On fail { Shall fail if calloc fails, check calloc error codes }
extern int snf_opcode_define_category(
    SNF_opcode_mmbr_t Code,
    const char *Definition);
/// @brief Defines an opcode category
/// @param Category OPcode's Category
/// @param Code OPcode subcategory's value
/// @param Definition String Definition for The sub-category
/// @return 0 On Success || 1 if Sub-Category Already Exists || -1 On calloc fail || -2 if Category was not found or opcode's data structure is empty ( Possibly SNF OPcode was not initialized )
extern int snf_opcode_define_sub_category(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t Code,
    const char *Definition);
/// @brief Defines an opcode category
/// @param Category OPcode's Category
/// @param SubCategory OPcode's Sub-Category
/// @param Code Opcode command's value
/// @param Definition String Definition for The command
/// @return  0 On Success || 1 if Command Already Exists || -1 On calloc fail || -2 if Command was not found or opcode's data structure is empty ( Possibly SNF OPcode was not initialized )
extern int snf_opcode_define_command(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Code,
    const char *Definition);
/// @brief Defines an opcode category
/// @param Category OPcode's Category
/// @param SubCategory OPcode's Sub-Category
/// @param Code Opcode command's value
/// @param Definition String Definition for The command
/// @return  0 On Success || 1 if Command Already Exists || -1 On calloc fail || -2 if Command was not found or opcode's data structure is empty ( Possibly SNF OPcode was not initialized )
extern int snf_opcode_define_detail(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Code,
    const char *Definition);

/// @brief Fetches the opcode Category from the opcode's data structure.
/// @param Category Category to be fetched.
/// @return Pointer to Category || NULL if Category wasn't found
extern SNF_opcode_LL_item *snf_opcode_get_category(
    SNF_opcode_mmbr_t Category);
/// @brief Fetches the opcode Sub-Category from the opcode's data structure.
/// @param Category Parent Category
/// @param SubCategory Sub-Category to be fetched.
/// @return Pointer to Sub-Category || NULL if Sub-Category wasn't found
extern SNF_opcode_LL_item *snf_opcode_get_sub_category(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory);
/// @brief Fetches the opcode Command from the opcode's data structure.
/// @param Category Parent Category
/// @param SubCategory Parent Sub-Category
/// @param Command Command to be fetched.
/// @return Pointer to Command || NULL if Command wasn't found
extern SNF_opcode_LL_item *snf_opcode_get_command(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command);
/// @brief Fetches the opcode Command from the opcode's data structure.
/// @param Category Parent Category
/// @param SubCategory Parent Sub-Category
/// @param Command Command to be fetched.
/// @return Pointer to Command || NULL if Command wasn't found
extern SNF_opcode_LL_item *snf_opcode_get_command(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command);
/// @param Category Parent Category
/// @param SubCategory Parent Sub-Category
/// @param Command Parent Command
/// @param Detail Detail to be fetched.
/// @return Pointer to Command detail || NULL if Command Detail wasn't found
extern SNF_opcode_LL_item *snf_opcode_get_detail(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Detail);
/// @brief Fetches a fully structured OPcode
/// @param Category Parent Category.
/// @param SubCategory Parent Sub-Category
/// @param Command Executing Command
/// @param Detail Given Detail.
/// @return OPcode || NULL if command not found
extern SNF_opcode *snf_opcode_get(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Detail);
/// @brief Fetches a fully structured OPcode using the default Detail
/// @param Category Parent Category.
/// @param SubCategory Parent Sub-Category
/// @param Command Executing Command
/// @return OPcode || NULL if command not found
extern SNF_opcode *snf_opcode_getu(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command);

/// @brief Compares between two opcodes.
/// @param op1
/// @param op2
/// @return -2 Given opcodes are different || -1 if either opcodes are NULL || 0 when opcodes are equal || 1 Opcodes have equal command. but op1 has bigger Detail || 2 Opcodes have equal command. but op2 has bigger Detail
extern int snf_opcode_compare(
    SNF_opcode *op1,
    SNF_opcode *op2);

#pragma region[Base Opcode Values]
//  Base Category for SNF
#define SNF_OPCODE_BASE_CAT (SNF_opcode_mmbr_t)0x00
//  Base Sub-Category for SNF
#define SNF_OPCODE_BASE_SUBCAT (SNF_opcode_mmbr_t)0x00
//  Default Detail
#define SNF_OPCODE_BASE_DET_UNDETAILED (SNF_opcode_mmbr_t)0x00

//  When client attemps to connect.
#define SNF_OPCODE_BASE_CMD_CONNECT (SNF_opcode_mmbr_t)0x00
//  When client attemps to reconnect - or forced to.
#define SNF_OPCODE_BASE_CMD_RECONNECT (SNF_opcode_mmbr_t)0x01
//  When client attemps to disconnect.
#define SNF_OPCODE_BASE_CMD_DISCONNECT (SNF_opcode_mmbr_t)0x02
//  When client requests SNF version of the Server.
#define SNF_OPCODE_BASE_CMD_SNF_VER (SNF_opcode_mmbr_t)0x03
//  When client was forced to disconnect ( Kicked ).
#define SNF_OPCODE_BASE_CMD_KICK (SNF_opcode_mmbr_t)0x04
//  When client's reuqest was confirmed.
#define SNF_OPCODE_BASE_CMD_CONFIRM (SNF_opcode_mmbr_t)0x05
//  When client's request was rejected.
#define SNF_OPCODE_BASE_CMD_REJECT (SNF_opcode_mmbr_t)0x06

#pragma region[Base Command: Invalid]
//  When client's request was invalid either, wrong version or incomplete request.
#define SNF_OPCODE_BASE_CMD_INVALID (SNF_opcode_mmbr_t)0xFF
//  Received opcode was not registred
#define SNF_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE (SNF_opcode_mmbr_t)0x01
//  Protocol used is invalid
#define SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL (SNF_opcode_mmbr_t)0x02
#pragma endregion
#pragma endregion

#pragma region[Base Opcode Function]

/// @brief Fetches the base opcode Category from the opcode's data structure.
/// @return Pointer to Category || NULL if Category wasn't found ( OPcode wasnt intialized )
extern SNF_opcode_LL_item *snf_opcode_get_base_category();
/// @brief Fetches the base opcode Sub-Category from the opcode's data structure.
/// @return Pointer to Sub-Category || NULL if Sub-Category wasn't found ( OPcode wasnt intialized )
extern SNF_opcode_LL_item *snf_opcode_get_base_sub_category();
/// @brief Fetches a base opcode Command from the opcode's data structure.
/// @return Pointer to Command || NULL if Command wasn't found
extern SNF_opcode_LL_item *snf_opcode_get_base_command(
    SNF_opcode_mmbr_t Command);
/// @brief Fetches a base opcode Command from the opcode's data structure.
/// @param Command Parent Command
/// @return Pointer to Command detail || NULL if Command Detail wasn't found
extern SNF_opcode_LL_item *snf_opcode_get_base_detail(
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Detail);
/// @brief Fetches a fully structured OPcode with a base "Command"
/// @return OPcode || NULL if command not found
extern SNF_opcode *snf_opcode_get_base(
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Detail);
/// @brief Fetches a fully structured OPcode witha base "Command" using the default Detail
/// @return OPcode || NULL if command not found
extern SNF_opcode *snf_opcode_getu_base(
    SNF_opcode_mmbr_t Command);
#pragma endregion

#endif