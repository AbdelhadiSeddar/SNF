#include "SNF/clt.h"
#include "SNF/request.h"
#include <SNF/epoll.h>
#include <SNF/hashtable.h>

SNF_ht *SNF_Sck_ht = NULL;
SNF_ht *SNF_Clt_ht = NULL;
size_t default_client_data_size = 0;
void snf_clt_init(int ht_min_Size) {
  if (!*snf_var_get(SNF_VAR_INITIALIZED, int))
    exit(EXIT_FAILURE);
  SNF_Clt_ht = snf_hashtable_inis(ht_min_Size);
  SNF_Sck_ht = snf_hashtable_inis(ht_min_Size);
  default_client_data_size = snf_var_getv(SNF_VAR_CLTS_DATA_SIZE, size_t);
}

SNF_CLT *snf_clt_new(int Sockfd) {
  SNF_CLT *Client = malloc(sizeof(SNF_CLT));
  pthread_mutex_init(&(Client->mutex), NULL);
  Client->sock = Sockfd;
  Client->mode = SNF_CLT_MODE_REGULAR;
  strcpy(Client->UUID, "00000000-0000-0000-0000-000000000000");
  if (default_client_data_size > 0)
    Client->data = calloc(1, default_client_data_size);
  else
    Client->data = NULL;

  return Client;
}

void snf_clt_free(SNF_CLT *Client) {
  if (Client != NULL) {
    if (!(Client->data))
      free(Client->data);
    free(Client);
  }
}

SNF_CLT *snf_clt_get_sockfd(int Sockfd) {
  return snf_clt_get_uuid(snf_clt_get_uuid_sockfd(Sockfd));
}

SNF_CLT *snf_clt_get_uuid(const char *uuid) {
  if (!SNF_Clt_ht) {
    snf_print_err("Client Storage is not defined!");
    exit(EXIT_FAILURE);
  }
  if (uuid == NULL)
    return NULL;
  SNF_ht_item *item = snf_hashtable_lookup(SNF_Clt_ht, uuid);
  return item == NULL ? NULL : (SNF_CLT *)item->Content;
}

char *snf_clt_get_uuid_sockfd(int Sockfd) {
  if (!SNF_Sck_ht) {
    snf_print_err("Socket Storage is not defined!");
    exit(EXIT_FAILURE);
  }
  SNF_ht_item *item = snf_hashtable_lookup(SNF_Sck_ht, (char[]){Sockfd, 0});
  if (item == NULL || item->Content == NULL)
    return NULL;
  return (char *)item->Content;
}

int snf_clt_check_sockfd(int Sockfd) {
  return snf_hashtable_lookup(SNF_Sck_ht, (char[]){Sockfd, 0}) != NULL;
}

int snf_clt_check_uuid(const char *uuid) {
  return snf_clt_get_uuid(uuid) != NULL;
}

void *snf_clt_handle_new(void *arg) {
  if (SNF_Clt_ht == NULL)
    return NULL;

  SNF_CLT *Client = (SNF_CLT *)arg;
  checkerr(Client == NULL ? -1 : 0, "Can not handle new client: Null");

  char *init_req;
  size_t init_req_size;
  snf_request_initial_get((void **)&init_req, &init_req_size);

  snf_snd(Client, init_req, init_req_size);
  free(init_req);

  snf_epoll_add(Client->sock);
  snf_hashtable_insert(SNF_Sck_ht, (char[]){Client->sock, '\0'}, NULL);
  // TODO: Handle the rest inside the snf_clt_handle()

  snf_epoll_add(SNF_SERVER_SOCKET);
  return NULL;
}

void *snf_clt_handle(void *arg) {
  if (arg == NULL)
    return NULL;
  int ClientSck = *(int *)arg;
  SNF_ht_item *Slot;
  SNF_CLT *Client;
  SNF_CLT *Original = snf_clt_get_sockfd(ClientSck);

  if (Original == NULL) {
    // It means that the sock is registed but the client itself isn't hence by
    // default, the connection is in the Initial Client Request stage or in
    // ONESHOT mode.
    Client = snf_clt_new(ClientSck);
    SNF_opcode *op = calloc(1, sizeof(SNF_opcode));
    snf_rcv(Client, (char *)(uint8_t *)op->opcode, 4);
    if (snf_opcode_compare(
            op, snf_opcode_get_base(SNF_OPCODE_BASE_CMD_CONNECT, 0x00)) >= 0) {
      switch (op->strct.Detail) {
      case SNF_OPCODE_BASE_DET_CONNECT_MULTISHOT:
        Client->mode = SNF_CLT_MODE_MULTISHOT;
        snf_rcv(Client, (char *)&(Client->req_count), 4);
      case SNF_OPCODE_BASE_DET_UNDETAILED:
        snf_clt_register(Client);
      case SNF_OPCODE_BASE_DET_CONNECT_ONESHOT:
        if (!(Client->mode))
          Client->mode = SNF_CLT_MODE_ONESHOT;
        break;
      default:
        snf_clt_disconnect(Client);
        break;
      }
    } else
      // TODO: Implement Reconnection, Now assumed same as deconnection.
      snf_clt_disconnect(Client);
  } else {
    char *UUID = calloc(37, sizeof(char));
    // INFO: This part Handles identifiying the already existing client that is
    // either in SNF_CLT_MODE_REGULAR || SNF_CLT_MODE_MULTISHOT
    int i = snf_rcv(&((SNF_CLT){.sock = ClientSck}), UUID, 36);
    if (
        // Check for correct size
        i < 36 ||
        // Insure the client is actually registred
        (Original = snf_clt_get_uuid(UUID)) != NULL ||
        // Insure the fetched UUID matches with the linked port number
        Original->sock != ClientSck) {
      // this is in case the conencted client has failed all of these
      // instances meaning an imposter.
      free(UUID);
      goto end_clt_handle;
    }
    free(UUID);
    Client = Original;
  }

  pthread_mutex_lock(&(Client->mutex));

  SNF_RQST *Rqst = snf_request_fetch_metadata(Client);
  Rqst = snf_request_handle(Client, Rqst);
  if (Client->mode == SNF_CLT_MODE_MULTISHOT) {
    Client->req_count--;
    if (Client->req_count < 1) {
      snf_hashtable_delete(SNF_Clt_ht, Client->UUID);
      snf_hashtable_delete(SNF_Sck_ht, (char[]){ClientSck, 0});
    }
  }
  pthread_mutex_unlock(&(Original->mutex));
  snf_epoll_add(Client->sock);

end_clt_handle:;
  switch (Client->mode) {
  case SNF_CLT_MODE_MULTISHOT:
    if (Client->req_count)
      break;
    else {
      snf_hashtable_delete(SNF_Clt_ht, Client->UUID);
    }
  case SNF_CLT_MODE_ONESHOT:
    snf_hashtable_delete(SNF_Sck_ht, (char[]){ClientSck, 0});
    snf_clt_disconnect(Client);
    if (SNF_CLT_MODE_ONESHOT)
      snf_clt_free(Client);
  default:
    break;
  }
  return NULL;
}

void snf_clt_register(SNF_CLT *Client) {
  uuid_t GUID;
  uuid_generate_time_safe(GUID);
  uuid_unparse_lower(GUID, Client->UUID);
  snf_hashtable_insert(SNF_Clt_ht, Client->UUID, Client);
  snf_hashtable_insert(SNF_Clt_ht, (char[]){Client->sock, '\0'}, Client->UUID);
}

void snf_clt_reconnect(SNF_CLT *Client) {
  snf_rcv(Client, Client->UUID, 36);
  SNF_CLT *Original = snf_hashtable_lookup(SNF_Clt_ht, Client->UUID)->Content;
  if (Original != NULL) {
    if (Client->sock < SNF_SERVER_SOCKET) {
      Original->sock = -1;
      return;
    }
    snf_epoll_add(Client->sock);
    snf_request_send_confirm(Client, NULL);
  } else {
    snf_request_send_reject(Client, NULL);
    snf_clt_register(Client);
  }
}

void snf_clt_disconnect(SNF_CLT *Client) {
  close(Client->sock);
  Client->sock = -1;
}
