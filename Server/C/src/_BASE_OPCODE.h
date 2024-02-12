#ifndef _BASE_OPCODE_H
#define _BASE_OPCODE_H
#define _SRBNP_VER 0000

#include <SRBNP/opcode.h>

#define SRBNP_OPCODE_BASE_CAT       (SRBNP_opcode_membr_t)0x00
#define SRBNP_OPCODE_BASE_SUBCAT    (SRBNP_opcode_membr_t)0x00

#define SRBNP_OPCODE_BASE_CMD_CONNECT       (SRBNP_opcode_membr_t)0x00
#define SRBNP_OPCODE_BASE_CMD_RECONNECT     (SRBNP_opcode_membr_t)0x01
#define SRBNP_OPCODE_BASE_CMD_DISCONNECT    (SRBNP_opcode_membr_t)0x02
#define SRBNP_OPCODE_BASE_CMD_SRBNP_VER     (SRBNP_opcode_membr_t)0x03
#define SRBNP_OPCODE_BASE_CMD_KICK          (SRBNP_opcode_membr_t)0x04

#define SRBNP_OPCODE_BASE_CMD_CONFIRM     (SRBNP_opcode_membr_t)0x05
#define SRBNP_OPCODE_BASE_CMD_REJECT      (SRBNP_opcode_membr_t)0x06

#pragma region [Base Command: Invalid]
#define SRBNP_OPCODE_BASE_CMD_INVALID       (SRBNP_opcode_membr_t)0xFF
#define SRBNP_OPCODE_DET_INVALID_UNDETAILED (SRBNP_opcode_membr_t)0x00
#define SRBNP_OPCODE_DET_INVALID_UNREGISTRED_OPCODE (SRBNP_opcode_membr_t)0x01
#define SRBNP_OPCODE_DET_INVALID_ERROR_PROTOCOL     (SRBNP_opcode_membr_t)0x02
#pragma endregion
#endif
