//////////////////////////////////////////////////////////////
///
/// \file opcode.h
/// This file Defines everything related to Opcodes ( Short for 
/// **OP**eartion**Codes** )
///
/// //////////////////////////////////////////////////////////

#ifndef opcode_h
#define opcode_h

#include <SNF/SNF.h>
/// @brief Defines SNF_opcode_mmbr_t 's size
#define SNF_opcode_mmbr_t uint8_t
/// @brief Defines SNF_opcode_mmbr_t 's maximum possible Value
#define SNF_opcode_mmbr_MAX UINT8_MAX

/// @brief Shortened definiton of struct SNF_opcode_t .
typedef union SNF_opcode_t SNF_opcode;
/// @brief Shortened definiton of struct SNF_opcode_LL_item_t .
typedef struct SNF_opcode_LL_item_t SNF_opcode_LL_item;

/// @brief Structure for the opcode 
/// @note If you wanna access the contents 
///         * For struct use ***strct*** member See SNF_opcode_struct
///         * For a Table use ***opcode*** member See SNF_opcode::opcode
union SNF_opcode_t
{
    /// @brief SNF_opcode's Structure if you wanna access it as a struct
    struct SNF_opcode_struct
    {
        /// @brief opcode's Category
        SNF_opcode_mmbr_t Category;
        /// @brief opcode's Sub-Category
        SNF_opcode_mmbr_t SubCategory;
        /// @brief opcode's Command
        SNF_opcode_mmbr_t Command;
        /// @brief opcode's Detail
        SNF_opcode_mmbr_t Detail;
    } strct;
    /// @brief SNF_opcode's Structure if you wanna access as a table
    /// @note   The content of opcode goest as the following
    ///         * Index <strong>0 -></strong> Category
    ///         * Index <strong>1 -></strong> Sub-Category
    ///         * Index <strong>2 -></strong> Command
    ///         * Index <strong>3 -></strong> Detail
    SNF_opcode_mmbr_t opcode[4];
};

#include <SNF/request.h>
/// @brief Structure used to save registred opcode members 
/// @note Ranks are:
///         * Category 
///         * Sub Category 
///         * Command 
///         * Detail
/// @warning    Don't define opcode members by allocating this structure manually, use appropriate functions snf_define_< \ref SNF_opcode_LL_item_t "Rank" >(..),
///             <br><strong>Example :</strong> for Categories use \ref snf_opcode_define_category()
struct SNF_opcode_LL_item_t
{
    /// @brief opcode Member value
    SNF_opcode_mmbr_t OPmmbr;
    /// @brief opcode Member's definition
    char *Definition;
    /// @brief Function to be called when the registred command is called
    /// @note by default it will call snf_cmd_unimplemented()
    SNF_RQST *(*func)(SNF_RQST *);
    /// @brief the next opcode member of the same \ref SNF_opcode_LL_item_t "Rank" (and of same Parent if they have one).
    SNF_opcode_LL_item *next;
    /// @brief Parent ( or higher in Rank ) opcode Member
    /// @note Category Rank opcode member should have this member with a value of **NULL** 
    SNF_opcode_LL_item *parent;
    /// @brief Child ( or lower in Rank ) opcode Member
    /// @note Detail Rank opcode member should have this member with a value of **NULL** 
    SNF_opcode_LL_item *sub;
};

/// @brief Data structure where opcodes will be saved.
extern SNF_opcode_LL_item *SNF_opcode_LL;
/// @brief used to check if SNF's base opcodes are initialized
extern int SNF_opcode_base_isinit;


/// @brief Initializes the SNF's opcodes
/// @return The possible results :
///         * **-1** On fail (Shall fail if calloc fails, check errno depending on calloc error codes)
///         * **0** On Success
extern int snf_opcode_init();
/// @brief Defines an opcode Category
/// @param Code OPcode category's value
/// @param Definition String Definition for The category
/// @return The possible results :
///         * **0** On Success 
///         * **1** if the Category already exists
///         * **-1** On fail ( Shall fail if calloc fails, check calloc error codes )
extern int snf_opcode_define_category(
    SNF_opcode_mmbr_t Code,
    const char *Definition);
/// @brief Defines an opcode Sub-Category
/// @param Category OPcode's Category
/// @param Code OPcode subcategory's value
/// @param Definition String Definition for The sub-category
/// @return The possible results :
///         * **0** On Success 
///         * **1** if Sub-Category Already Exists
///         * **-1** On calloc fail
///         * **-2** if Category was not found or opcode's data structure is empty ( Possibly snf_opcode_init was not called )
extern int snf_opcode_define_sub_category(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t Code,
    const char *Definition);
/// @brief Defines an opcode Command
/// @param Category OPcode's Category
/// @param SubCategory OPcode's Sub-Category
/// @param Code Opcode command's value
/// @param Definition String Definition for The command/// @return The possible results :
///         * **0** On Success 
///         * **1** if Command Already Exists
///         * **-1** On calloc fail
///         * **-2** if Sub-Category was not found or opcode's data structure is empty ( Possibly snf_opcode_init was not called )
extern int snf_opcode_define_command(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Code,
    const char *Definition,
    SNF_RQST *(func)(SNF_RQST *));
/// @brief Defines an opcode Detail
/// @param Category OPcode's Category
/// @param SubCategory OPcode's Sub-Category
/// @param Command Opcode's Command
/// @param Code Opcode detail's value
/// @param Definition String Definition for The detail
/// @return The possible results :
///         * **0** On Success 
///         * **1** if Detail Already Exists
///         * **-1** On calloc fail
///         * **-2** if Command was not found or opcode's data structure is empty ( Possibly snf_opcode_init was not called )
extern int snf_opcode_define_detail(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Code,
    const char *Definition);

/// @brief Fetches the opcode Category from the opcode's data structure.
/// @param Category Category to be fetched.
/// @return The possible results :
///         * **NULL** if Category wasn't found or opcode's data structure is empty ( Possibly snf_opcode_init was not called )
///         * **Pointer** to the Category 
extern SNF_opcode_LL_item *snf_opcode_get_category(
    SNF_opcode_mmbr_t Category);
/// @brief Fetches the opcode Sub-Category from the opcode's data structure.
/// @param Category Parent Category
/// @param SubCategory Sub-Category to be fetched.
/// @return The possible results :
///         * **NULL** if Sub-Category wasn't found
///         * **Pointer** to the Sub-Category or opcode's data structure is empty ( Possibly snf_opcode_init was not called )
extern SNF_opcode_LL_item *snf_opcode_get_sub_category(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory);
/// @brief Fetches the opcode Command from the opcode's data structure.
/// @param Category Parent Category
/// @param SubCategory Parent Sub-Category
/// @param Command Command to be fetched.
/// @return The possible results :
///         * **NULL** if Command wasn't found
///         * **Pointer** to the Command or opcode's data structure is empty ( Possibly snf_opcode_init was not called )
extern SNF_opcode_LL_item *snf_opcode_get_command(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command);
/// @param Category Parent Category
/// @param SubCategory Parent Sub-Category
/// @param Command Parent Command
/// @param Detail Detail to be fetched.
/// @return The possible results :
///         * **NULL** if Detail wasn't found
///         * **Pointer** to the Detail 
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
/// @return The possible results :
///         * **NULL** if any of the Members was not found
///         * **Pointer** to the \ref SNF_opcode instance
/// @note If you want to generate an Undetailed ( Detail's value is equal to \ref SNF_OPCODE_BASE_DET_UNDETAILED ) then use
///         \ref snf_opcode_getu() 
extern SNF_opcode *snf_opcode_get(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Detail);
/// @brief Fetches a fully structured OPcode using the default Detail
/// @param Category Parent Category.
/// @param SubCategory Parent Sub-Category
/// @param Command Executing Command
/// @return The possible results :
///         * **NULL** if any of the Members was not found
///         * **Pointer** to the undetailed \ref SNF_opcode instance
/// @note Detail's value will be equal to \ref SNF_OPCODE_BASE_DET_UNDETAILED
extern SNF_opcode *snf_opcode_getu(
    SNF_opcode_mmbr_t Category,
    SNF_opcode_mmbr_t SubCategory,
    SNF_opcode_mmbr_t Command);

/// @brief Compares between two opcodes.
/// @param op1 OPCode 1
/// @param op2 OPCode 2
/// @return The possible results :
///         * **-2 Given opcodes are different
///         * **-1 if either opcodes are NULL
///         * **0** when opcodes are equal
///         * **1** Opcodes have equal command. but op1 has bigger Detail
///         * **2** Opcodes have equal command. but op2 has bigger Detail
extern int snf_opcode_compare(
    SNF_opcode *op1,
    SNF_opcode *op2);
extern int snf_opcode_isbase(
    SNF_opcode *op);
#pragma region[Base Opcode Values]
/// @brief Base Category for SNF
/// @warning Do not add anything under this Catergory
#define SNF_OPCODE_BASE_CAT (SNF_opcode_mmbr_t)0x00
/// @brief Base Sub-Category for SNF
/// @warning Do not add anything under this Sub-Catergory
#define SNF_OPCODE_BASE_SUBCAT (SNF_opcode_mmbr_t)0x00
/// @brief  Default Detail
/// @note This will always be added by default to every Command
#define SNF_OPCODE_BASE_DET_UNDETAILED (SNF_opcode_mmbr_t)0x00

#pragma region[Base Command: Connect]
/// @brief  When client attemps to connect.
/// @warning Do not add anything under this Command
#define SNF_OPCODE_BASE_CMD_CONNECT (SNF_opcode_mmbr_t)0x00

#define SNF_OPCODE_BASE_DET_CONNECT_ONESHOT   (SNF_opcode_mmbr_t)0x01
#define SNF_OPCODE_BASE_DET_CONNECT_MULTISHOT (SNF_opcode_mmbr_t)0x02
#pragma endregion

/// @brief  When client attemps to reconnect - or forced to.
/// @warning Do not add anything under this Command
#define SNF_OPCODE_BASE_CMD_RECONNECT (SNF_opcode_mmbr_t)0x01
/// @brief  When client attemps to disconnect.
/// @warning Do not add anything under this Command
#define SNF_OPCODE_BASE_CMD_DISCONNECT (SNF_opcode_mmbr_t)0x02

#pragma region[Base Command: VER_INF]
/// @brief  When client requests The Protocol's version of the Server.
/// @warning Do not add anything under this Command
#define SNF_OPCODE_BASE_CMD_VER_INF (SNF_opcode_mmbr_t)0x03

#define SNF_OPCODE_BASE_DET_VER_INF_VER_IMPL  (SNF_opcode_mmbr_t)0x01
#pragma endregion

/// @brief  When client was forced to disconnect ( Kicked ).
/// @warning Do not add anything under this Command
#define SNF_OPCODE_BASE_CMD_KICK (SNF_opcode_mmbr_t)0x04
/// @brief  When client's reuqest was confirmed.
/// @warning Do not add anything under this Command
#define SNF_OPCODE_BASE_CMD_CONFIRM (SNF_opcode_mmbr_t)0x05
/// @brief  When client's request was rejected.
/// @warning Do not add anything under this Command
#define SNF_OPCODE_BASE_CMD_REJECT (SNF_opcode_mmbr_t)0x06

#pragma region[Base Command: Invalid]
/// @brief When client's request was invalid either, wrong version or incomplete request.
/// @warning Do not add anything under this Command
#define SNF_OPCODE_BASE_CMD_INVALID (SNF_opcode_mmbr_t)0xFF
/// @brief Received opcode was not registred
#define SNF_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE (SNF_opcode_mmbr_t)0x01
/// @brief Protocol used is invalid
#define SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL (SNF_opcode_mmbr_t)0x02
/// @brief Received opcode does not have a function to call
#define SNF_OPCODE_BASE_DET_INVALID_UNIMPLEMENTED_OPCODE (SNF_opcode_mmbr_t)0x03
#pragma endregion
#pragma endregion

#pragma region[Base Opcode Function]

/// @brief Fetches the base opcode Category from the opcode's data structure.
/// @return The possible results :
///         * **NULL** if Category wasn't found or opcode's data structure is empty ( Possibly snf_opcode_init was not called )
///         * **Pointer** to the Category 
extern SNF_opcode_LL_item *snf_opcode_get_base_category();
/// @brief Fetches the base opcode Sub-Category from the opcode's data structure.
/// @return The possible results :
///         * **NULL** if Sub-Category wasn't found
///         * **Pointer** to the Sub-Category or opcode's data structure is empty ( Possibly snf_opcode_init was not called )
extern SNF_opcode_LL_item *snf_opcode_get_base_sub_category();
/// @brief Fetches a base opcode Command from the opcode's data structure.
/// @param Command requested Command
/// @return The possible results :
///         * **NULL** if Command wasn't found
///         * **Pointer** to the Command or opcode's data structure is empty ( Possibly snf_opcode_init was not called )
extern SNF_opcode_LL_item *snf_opcode_get_base_command(
    SNF_opcode_mmbr_t Command);
/// @brief Fetches a base opcode Command from the opcode's data structure.
/// @param Command Parent Command
/// @param Detail Command's Detail
/// @return The possible results :
///         * **NULL** if Detail wasn't found
///         * **Pointer** to the Detail or opcode's data structure is empty ( Possibly snf_opcode_init was not called )
extern SNF_opcode_LL_item *snf_opcode_get_base_detail(
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Detail);
/// @brief Fetches a fully structured OPcode with a base "Command"
/// @param Command Base Command's Value
/// @param Detail Base Command Detail's Value
/// @return The possible results :
///         * **NULL** if any of the Members was not found
///         * **Pointer** to the \ref SNF_opcode instance
/// @note If you want to generate an Undetailed ( Detail's value is equal to \ref SNF_OPCODE_BASE_DET_UNDETAILED ) then use
///         \ref snf_opcode_getu_base() 
extern SNF_opcode *snf_opcode_get_base(
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Detail);
/// @brief Fetches a fully structured OPcode witha base "Command" using the default Detail
/// @param Command Base Command's Value
/// @return The possible results :
///         * **NULL** if any of the Members was not found
///         * **Pointer** to the undetailed \ref SNF_opcode instance
/// @note Detail's value will be equal to \ref SNF_OPCODE_BASE_DET_UNDETAILED
extern SNF_opcode *snf_opcode_getu_base(
    SNF_opcode_mmbr_t Command);
#pragma endregion

#endif
