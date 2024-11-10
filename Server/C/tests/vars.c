#include <SNF.h>


int main() {

    snf_var_default();
    int i = -1;

    snf_var_set(SNF_VAR_EPOLL_MAXEVENTS, &i);

    i = -2;
    snf_var_set(SNF_VAR_OPCODE_INIS, &i);


    return 0;
}