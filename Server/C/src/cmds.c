#include "SNF/cmds.h"

SNF_RQST *snf_cmd_invalid_error_protocol(SNF_RQST *Request)
{
    return snf_request_gen_response(
        Request,
        snf_opcode_get_base(
            SNF_OPCODE_BASE_CMD_INVALID,
            SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL
        ),
        NULL
    );
}

SNF_RQST *snf_cmd_invalid_unregistred(SNF_RQST *Request)
{
    return snf_request_gen_response(
        Request,
        snf_opcode_get_base(
            SNF_OPCODE_BASE_CMD_INVALID,
            SNF_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE
        ),
        NULL
    );
}

SNF_RQST *snf_cmd_invalid_unimplemented(SNF_RQST *Request)
{
    return snf_request_gen_response(
        Request,
        snf_opcode_get_base(
            SNF_OPCODE_BASE_CMD_INVALID,
            SNF_OPCODE_BASE_DET_INVALID_UNIMPLEMENTED_OPCODE
        ),
        NULL
    );
}

SNF_RQST *snf_cmd_snf_ver(SNF_RQST *Request)
{
    return snf_request_gen_response(
        Request,
        snf_opcode_getu_base(
            SNF_OPCODE_BASE_CMD_CONFIRM
        ),
        snf_request_arg_gen(_SNF_VER)
    );
}
