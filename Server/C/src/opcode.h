#ifndef opcode_h
#define opcode_h

#include <SRBNP/SRBNP.h>

#define SRBNP_opcode_mmbr_t uint8_t
#define SRBNP_opcode_mmbr_MAX UINT8_MAX

/// @brief Structure for SRBNP's opcodes.
typedef union SRBNP_opcode_t SRBNP_opcode;
/// @brief Structure for saving SRBNP's categories, sub-categories, commands
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
    } strct;
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


/// @brief Data structure where opcodes will be saved.
extern SRBNP_opcode_LL_item *SRBNP_opcode_LL;
/// @brief used to check if SRBNP's base opcodes are initialized
extern int SRBNP_opcode_base_isinit;
/// @brief Initializes the SRBNP's opcodes
/// @return 0 On Success || -1 On fail { Shall fail if calloc fails, check calloc error codes }
extern int srbnp_opcode_init();
/// @brief Defines an opcode category
/// @param Code OPcode category's value
/// @param Definition String Definition for The category
/// @return 0 On Success || 1 if Category Already Exists || -1 On fail { Shall fail if calloc fails, check calloc error codes }
extern int srbnp_opcode_define_category(
    SRBNP_opcode_mmbr_t Code,
    const char *Definition);
/// @brief Defines an opcode category
/// @param Category OPcode's Category
/// @param Code OPcode subcategory's value
/// @param Definition String Definition for The sub-category
/// @return 0 On Success || 1 if Sub-Category Already Exists || -1 On calloc fail || -2 if Category was not found or opcode's data structure is empty ( Possibly SRBNP OPcode was not initialized )
extern int srbnp_opcode_define_sub_category(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t Code,
    const char *Definition);
/// @brief Defines an opcode category
/// @param Category OPcode's Category
/// @param SubCategory OPcode's Sub-Category
/// @param Code Opcode command's value
/// @param Definition String Definition for The command
/// @return  0 On Success || 1 if Command Already Exists || -1 On calloc fail || -2 if Command was not found or opcode's data structure is empty ( Possibly SRBNP OPcode was not initialized )
extern int srbnp_opcode_define_command(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Code,
    const char *Definition);
/// @brief Defines an opcode category
/// @param Category OPcode's Category
/// @param SubCategory OPcode's Sub-Category
/// @param Code Opcode command's value
/// @param Definition String Definition for The command
/// @return  0 On Success || 1 if Command Already Exists || -1 On calloc fail || -2 if Command was not found or opcode's data structure is empty ( Possibly SRBNP OPcode was not initialized )
extern int srbnp_opcode_define_detail(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Command,
    SRBNP_opcode_mmbr_t Code,
    const char *Definition);

/// @brief Fetches the opcode Category from the opcode's data structure. 
/// @param Category Category to be fetched.
/// @return Pointer to Category || NULL if Category wasn't found
extern SRBNP_opcode_LL_item *srbnp_opcode_get_category(
    SRBNP_opcode_mmbr_t Category);
/// @brief Fetches the opcode Sub-Category from the opcode's data structure. 
/// @param Category Parent Category
/// @param SubCategory Sub-Category to be fetched.
/// @return Pointer to Sub-Category || NULL if Sub-Category wasn't found 
extern SRBNP_opcode_LL_item *srbnp_opcode_get_sub_category(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory);
/// @brief Fetches the opcode Command from the opcode's data structure. 
/// @param Category Parent Category
/// @param SubCategory Parent Sub-Category
/// @param Command Command to be fetched.
/// @return Pointer to Command || NULL if Command wasn't found 
extern SRBNP_opcode_LL_item *srbnp_opcode_get_command(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Command);
    /// @brief Fetches the opcode Command from the opcode's data structure. 
/// @param Category Parent Category
/// @param SubCategory Parent Sub-Category
/// @param Command Command to be fetched.
/// @return Pointer to Command || NULL if Command wasn't found 
extern SRBNP_opcode_LL_item *srbnp_opcode_get_command(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Command);
/// @param Category Parent Category
/// @param SubCategory Parent Sub-Category
/// @param Command Parent Command
/// @param Detail Detail to be fetched.
/// @return Pointer to Command detail || NULL if Command Detail wasn't found 
extern SRBNP_opcode_LL_item *srbnp_opcode_get_detail(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Command,
    SRBNP_opcode_mmbr_t Detail);
/// @brief Fetches a fully structured OPcode
/// @param Category Parent Category.
/// @param SubCategory Parent Sub-Category
/// @param Command Executing Command
/// @param Detail Given Detail.
/// @return OPcode || NULL if command not found
extern SRBNP_opcode *srbnp_opcode_get(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Command,
    SRBNP_opcode_mmbr_t Detail);
/// @brief Fetches a fully structured OPcode using the default Detail
/// @param Category Parent Category.
/// @param SubCategory Parent Sub-Category
/// @param Command Executing Command
/// @return OPcode || NULL if command not found
extern SRBNP_opcode *srbnp_opcode_getu(
    SRBNP_opcode_mmbr_t Category,
    SRBNP_opcode_mmbr_t SubCategory,
    SRBNP_opcode_mmbr_t Command);

/// @brief Compares between two opcodes.
/// @param op1 
/// @param op2 
/// @return -2 Given opcodes are different || -1 if either opcodes are NULL || 0 when opcodes are equal || 1 Opcodes have equal command. but op1 has bigger Detail || 2 Opcodes have equal command. but op1 has bigger Detail
extern int srbnp_opcode_compare(
    SRBNP_opcode *op1,
    SRBNP_opcode *op2);

#pragma region[Base Opcode Values]
//  Base Category for SRBNP
#define SRBNP_OPCODE_BASE_CAT (SRBNP_opcode_mmbr_t)0x00
//  Base Sub-Category for SRBNP
#define SRBNP_OPCODE_BASE_SUBCAT (SRBNP_opcode_mmbr_t)0x00
//  Default Detail 
#define SRBNP_OPCODE_BASE_DET_UNDETAILED (SRBNP_opcode_mmbr_t)0x00

//  When client attemps to connect.
#define SRBNP_OPCODE_BASE_CMD_CONNECT (SRBNP_opcode_mmbr_t)0x00
//  When client attemps to reconnect - or forced to.
#define SRBNP_OPCODE_BASE_CMD_RECONNECT (SRBNP_opcode_mmbr_t)0x01
//  When client attemps to disconnect.
#define SRBNP_OPCODE_BASE_CMD_DISCONNECT (SRBNP_opcode_mmbr_t)0x02
//  When client requests SRBNP version.
#define SRBNP_OPCODE_BASE_CMD_SRBNP_VER (SRBNP_opcode_mmbr_t)0x03
//  When client was forced to disconnect ( Kicked ).
#define SRBNP_OPCODE_BASE_CMD_KICK (SRBNP_opcode_mmbr_t)0x04
//  When client's reuqest was confirmed.
#define SRBNP_OPCODE_BASE_CMD_CONFIRM (SRBNP_opcode_mmbr_t)0x05
//  When client's request was rejected.
#define SRBNP_OPCODE_BASE_CMD_REJECT (SRBNP_opcode_mmbr_t)0x06

#pragma region[Base Command: Invalid]
//  When client's request was invalid either, wrong version or incomplete request.
#define SRBNP_OPCODE_BASE_CMD_INVALID (SRBNP_opcode_mmbr_t)0xFF
//  Received opcode was not registred
#define SRBNP_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE (SRBNP_opcode_mmbr_t)0x01
//  Protocol used is invalid
#define SRBNP_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL (SRBNP_opcode_mmbr_t)0x02
#pragma endregion
#pragma endregion

#pragma region[Base Opcode Function]

/// @brief Fetches the base opcode Category from the opcode's data structure. 
/// @return Pointer to Category || NULL if Category wasn't found ( OPcode wasnt intialized )
extern SRBNP_opcode_LL_item *srbnp_opcode_get_base_category();
/// @brief Fetches the base opcode Sub-Category from the opcode's data structure. 
/// @return Pointer to Sub-Category || NULL if Sub-Category wasn't found ( OPcode wasnt intialized )
extern SRBNP_opcode_LL_item *srbnp_opcode_get_base_sub_category();
/// @brief Fetches a base opcode Command from the opcode's data structure. 
/// @return Pointer to Command || NULL if Command wasn't found
extern SRBNP_opcode_LL_item *srbnp_opcode_get_base_command(
    SRBNP_opcode_mmbr_t Command);
/// @brief Fetches a base opcode Command from the opcode's data structure. 
/// @param Command Parent Command
/// @return Pointer to Command detail || NULL if Command Detail wasn't found 
extern SRBNP_opcode_LL_item *srbnp_opcode_get_base_detail(
    SRBNP_opcode_mmbr_t Command,
    SRBNP_opcode_mmbr_t Detail);
/// @brief Fetches a fully structured OPcode with a base "Command"
/// @return OPcode || NULL if command not found
extern SRBNP_opcode *srbnp_opcode_get_base(
    SRBNP_opcode_mmbr_t Command,
    SRBNP_opcode_mmbr_t Detail);
/// @brief Fetches a fully structured OPcode witha base "Command" using the default Detail
/// @return OPcode || NULL if command not found
extern SRBNP_opcode *srbnp_opcode_getu_base(
    SRBNP_opcode_mmbr_t Command);
/// @brief Fetches a fully structured "Invalid" OPcode
/// @param Detail Invalid COmmand's detail
/// @return OPcode || NULL if command not found
extern SRBNP_opcode *srbnp_opcode_get_invalid(
    SRBNP_opcode_mmbr_t Detail);
/// @brief Fetches a fully structured "Invalid" OPcode using the default Detail
/// @param Detail Invalid COmmand's detail
/// @return OPcode || NULL if command not found
extern SRBNP_opcode *srbnp_opcode_getu_invalid();
#pragma endregion

#endif