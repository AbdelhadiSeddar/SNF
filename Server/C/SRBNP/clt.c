#include "_Imports.h"

clt *clt_new(int Sockfd)
{
    clt *Client = malloc(sizeof(clt));
    Client ->sock = Sockfd;
    return Client;
}

clt *clt_get_sockfd(int Sockfd)
{
    return local_db_fetch_clt(_ACTION_VAR_SOCK, (void*)&Sockfd);
}

clt *clt_get_uuid(const char *uuid)
{
    return local_db_fetch_clt(_ACTION_VAR_UUID, (void*)uuid);
}

int clt_check_sockfd(int Sockfd)
{
    return local_db_check_clt(_ACTION_VAR_SOCK, (void*)&Sockfd);
}
int clt_check_uuid(const char *uuid)
{
    return local_db_check_clt(_ACTION_VAR_UUID, (void*)uuid);
}
