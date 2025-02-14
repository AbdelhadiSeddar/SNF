/*
 *   Example Code
 */
#include "src/SNF/request.h"
#include <SNF.h>


int main(int argc, char **args)
{
    snf_var_default();
    
    snf_var_set(SNF_VAR_PROGRAM_NAME, "MainTest");
    snf_var_set(SNF_VAR_PROGRAM_VER, "0.0.0");

    snf_network_init();

    snf_request_initial_lock();
    snf_request_initial_compile(
      snf_opcode_getu_base(
        SNF_OPCODE_BASE_CMD_CONNECT
      ), 
      snf_request_arg_gen("Active"));
    snf_request_initial_unlock();
    snf_network_join();
}
