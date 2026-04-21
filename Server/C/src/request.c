#include "SNF/request.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>

char *__snf_request_initial = NULL;
size_t __snf_request_initial_size = 0;

static int __snf_request_initial_readers = 0;
static int __snf_request_initial_writers_waiting = 0;
static int __snf_request_initial_writer_active = 0;

static pthread_mutex_t __snf_request_initial_main_mutex =
    PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t __snf_request_initial_can_read = PTHREAD_COND_INITIALIZER;
static pthread_cond_t __snf_request_initial_can_write =
    PTHREAD_COND_INITIALIZER;

void snf_request_initial_init() {
  pthread_cond_signal(&__snf_request_initial_can_write);
  pthread_cond_broadcast(&__snf_request_initial_can_read);

  snf_request_initial_compile(snf_opcode_getu_base(SNF_OPCODE_BASE_CMD_CONNECT),
                              NULL);
}

void snf_request_initial_rlock() {
  pthread_mutex_lock(&__snf_request_initial_main_mutex);

  while (__snf_request_initial_writer_active ||
         __snf_request_initial_writers_waiting > 0) {
    pthread_cond_wait(&__snf_request_initial_can_read,
                      &__snf_request_initial_main_mutex);
  }

  __snf_request_initial_readers++;

  pthread_mutex_unlock(&__snf_request_initial_main_mutex);
}

void snf_request_initial_runlock() {
  pthread_mutex_lock(&__snf_request_initial_main_mutex);

  __snf_request_initial_readers--;

  if (__snf_request_initial_readers == 0) {
    if (__snf_request_initial_writers_waiting > 0) {
      pthread_cond_signal(&__snf_request_initial_can_write);
    }
  }

  pthread_mutex_unlock(&__snf_request_initial_main_mutex);
}

void snf_request_initial_wlock() {
  pthread_mutex_lock(&__snf_request_initial_main_mutex);

  __snf_request_initial_writers_waiting++;

  while (__snf_request_initial_readers > 0 ||
         __snf_request_initial_writer_active) {
    pthread_cond_wait(&__snf_request_initial_can_write,
                      &__snf_request_initial_main_mutex);
  }

  __snf_request_initial_writers_waiting--;
  __snf_request_initial_writer_active = 1;

  pthread_mutex_unlock(&__snf_request_initial_main_mutex);
}

void snf_request_initial_wunlock() {
  pthread_mutex_lock(&__snf_request_initial_main_mutex);

  __snf_request_initial_writer_active = 0;

  if (__snf_request_initial_writers_waiting > 0) {
    pthread_cond_signal(&__snf_request_initial_can_write);
  } else {
    pthread_cond_broadcast(&__snf_request_initial_can_read);
  }

  pthread_mutex_unlock(&__snf_request_initial_main_mutex);
}

void snf_request_initial_compile(SNF_opcode *op, SNF_RQST_ARG *arg) {

  size_t prot_ver_len = strlen(_SNF_SNP_VER),
         impl_ver_len = strlen(_SNF_VER) + strlen(_SNF) + 1,
         prog_ver_len = strlen(snf_var_get(SNF_VAR_PROGRAM_VER, char)) +
                        strlen(snf_var_get(SNF_VAR_PROGRAM_NAME, char)) + 1;
  size_t server_info_len = prot_ver_len + impl_ver_len + prog_ver_len + 3;

  uint32_t nargs = 0;
  size_t sargs = 0, s_split_args = 0;

  SNF_RQST_ARG *head = arg;
  while (head != NULL) {
    nargs++;
    // TODO: Argument Length Checked here as string!!
    sargs += strlen(head->arg);
    head = head->next;
    if (head != NULL)
      s_split_args += sizeof(char);
  }

  size_t req_len = 0;
  req_len = server_info_len    // Server Informations length + 3 @'s
            + 4                // Size of OPCode
            + sizeof(uint32_t) // Size of Size of Arguments
            + sizeof(uint32_t) // Size of Amount of Arguments
            + s_split_args     // Size of included splitters
            + sargs            // Size of actual request argument
      ;

  char *final = calloc(req_len + 1, sizeof(char));

  sprintf(final, "%s|%s@%s|%s@%s|", _SNF_SNP_VER, /* | */
          _SNF_VER, /*@*/ _SNF, snf_var_get(SNF_VAR_PROGRAM_VER, char),
          /*@*/ snf_var_get(SNF_VAR_PROGRAM_NAME, char));

  uint32_t final_written = server_info_len;

  // Preparing the rest of the metadata
  char *Metadata = calloc(4 * 3, sizeof(char));

  for (int i = 0; i < 4; i++)
    Metadata[i] = op->opcode[i];

  // Preparing the Number of arguments
  char *args = snf_uint32_to_bytes(nargs, 4);
  if (args != NULL) {
    for (int i = 0; i < 4; i++)
      Metadata[i + 4] = args[i];
    free(args);
  }

  // Preparing The size of arguments
  args = snf_uint32_to_bytes(sargs, 4);

  if (args != NULL) {
    for (int i = 0; i < 4; i++)
      Metadata[i + 8] = args[i];
    free(args);
  }

  // Inserting the arguments
  memcpy((char *)(final + final_written), Metadata, 12);
  free(Metadata);
  final_written += 12;

  args = (char *)(final + final_written);
  if (nargs > 0) {
    head = arg;
    while (head != NULL) {
      strcat(args, head->arg);

      head = head->next;

      if (head != NULL)
        strcat(args, UNIT_SCEPARATOR);
    }
  }

  snf_request_initial_wlock();

  __snf_request_initial_size = req_len;

  if (__snf_request_initial != NULL)
    free(__snf_request_initial);

  __snf_request_initial = final;

  snf_request_initial_wunlock();
}
void snf_request_initial_get(void **request, size_t *size) {
  snf_request_initial_rlock();
  *request = calloc(__snf_request_initial_size + 1, sizeof(char));
  memcpy(*request, __snf_request_initial, __snf_request_initial_size);
  *size = __snf_request_initial_size;
  snf_request_initial_runlock();
}

SNF_RQST *snf_request_fetch_metadata(SNF_CLT *Client) {
  SNF_RQST *re = snf_request_gen();
  char *Request;

  /**
   * Fetching OPCODE
   */
  snf_rcv(Client, re->OPCODE->opcode, 4);

  /**
   * Fetching Request UID
   */
  snf_rcv(Client, re->UID, strlen(NULLREQUEST) + 1);

  snf_rcv(Client, (char *)&re->n_args, 4);
  /**
   * Fetching Arguments Length & handling according to it
   */
  char MsgSize[4];
  snf_rcv(Client, MsgSize, 4);
  re->s_args = snf_bytes_to_uint32(MsgSize, 4);

  return re;
}

SNF_RQST_ARG *snf_request_fetch_arguments(SNF_CLT *Client, SNF_RQST *Rqst) {
  if (Client == NULL || Rqst == NULL)
    return NULL;
  SNF_RQST_ARG *Top_rqst = NULL;
  if (Rqst->s_args == 0)
    goto end_request_fetchfrom_clt;
  SNF_RQST_ARG *Current = NULL;
  /**
   * Fetching Arguments
   */
  char *Request = calloc(Rqst->s_args + 1, sizeof(char));
  snf_rcv(Client, Request, Rqst->s_args);
  /**
   * Separating Arguments
   */
  while (Rqst->s_args > 0) {
    int chr = (int)(strchrnul(Request, UNIT_SCEPARATOR[0]) - Request);
    char *tmpRqst = calloc(chr, sizeof(char));
    strncpy(tmpRqst, Request, chr);
    tmpRqst[chr] = '\0';
    SNF_RQST_ARG *new = snf_request_arg_gen(tmpRqst);
    if (Top_rqst == NULL)
      Top_rqst = new;
    if (Current == NULL)
      Current = new;
    else {
      Current->next = new;
      Current = new;
    }
    char *tmp = calloc(Rqst->s_args - chr - 1, sizeof(char));
    memcpy(tmp, Request + chr + 1, Rqst->s_args -= chr + 1);
    free(Request);
    Request = tmp;
  }
end_request_fetchfrom_clt:;
  return Top_rqst;
}
SNF_RQST *snf_request_gen() {
  SNF_RQST *re = calloc(1, sizeof(SNF_RQST));
  re->args = NULL;
  re->OPCODE = calloc(1, sizeof(SNF_opcode));
  return re;
}

SNF_RQST *snf_request_gen_wUID(const char UID[16]) {
  SNF_RQST *re = snf_request_gen();
  memcpy(re->UID, UID, 16);
  return re;
}

void snf_request_free(SNF_RQST *Request) {
  if (Request == NULL)
    return;
  snf_request_args_free(Request->args);
  free(Request);
}

SNF_RQST *snf_request_gen_response(SNF_RQST *Original, SNF_opcode *OPCODE,
                                   SNF_RQST_ARG *Args) {
  SNF_RQST *re = snf_request_gen_wUID(Original->UID);
  re->OPCODE = OPCODE;
  re->args = Args;
  return re;
}

SNF_RQST *snf_request_gen_server_OPCODE(SNF_opcode *OPCODE) {
  return snf_request_gen_response(snf_request_gen_wUID(NULLREQUEST), OPCODE,
                                  NULL);
}

SNF_RQST *snf_request_gen_base(SNF_RQST *Original, SNF_opcode_mmbr_t Command,
                               SNF_opcode_mmbr_t Detail) {
  return snf_request_gen_response(Original,
                                  snf_opcode_get_base(Command, Detail), NULL);
}
SNF_RQST *snf_request_genu_base(SNF_RQST *Original, SNF_opcode_mmbr_t Command) {
  return snf_request_gen_response(
      Original, snf_opcode_get_base(Command, SNF_OPCODE_BASE_DET_UNDETAILED),
      NULL);
}

int snf_request_get_nargs(SNF_RQST *args) {
  if (args == NULL)
    return 0;
  int nargs = 1;
  SNF_RQST_ARG *arg = args->args;
  if (arg == NULL)
    return 0;
  while (arg->next == NULL) {
    nargs++;
    arg = arg->next;
  }
  return nargs;
}

SNF_RQST_ARG *snf_request_arg_gen(const char *arg) {
  SNF_RQST_ARG *Arg = calloc(1, sizeof(SNF_RQST_ARG));
  Arg->arg = calloc(strlen(arg) + 1, sizeof(char));
  strcpy(Arg->arg, arg);
  return Arg;
}

void snf_request_arg_free(SNF_RQST_ARG *arg) {
  if (arg == NULL)
    return;
  free(arg->arg);
  free(arg);
}

void snf_request_args_free(SNF_RQST_ARG *arg) {
  if (arg == NULL)
    return;
  SNF_RQST_ARG *current = arg;
  SNF_RQST_ARG *next = current->next;
  while (next != NULL) {
    next = current->next;
    snf_request_arg_free(current);
    current = next;
  };
  arg = NULL;
}

void snf_request_arg_insert(SNF_RQST *Request, SNF_RQST_ARG *arg) {
  if (Request->args == NULL) {
    Request->args = arg;
  } else {
    SNF_RQST_ARG *last = Request->args;
    while (last->next != NULL)
      last = last->next;
    last->next = arg;
  }
}

void *snf_request_handle(SNF_CLT *Client, SNF_RQST *Rqst) {
  if (Rqst != NULL) {

    SNF_RQST *re = NULL;
    if (snf_opcode_isbase(Rqst->OPCODE) &&
        Rqst->OPCODE->strct.Command == SNF_OPCODE_BASE_CMD_DISCONNECT) {
      snf_clt_disconnect(Client);
      return NULL;
    } else {
      snf_request_arg_insert(Rqst, snf_request_fetch_arguments(Client, Rqst));
      SNF_opcode_LL_item *item = snf_opcode_get_command(
          Rqst->OPCODE->strct.Category, Rqst->OPCODE->strct.SubCategory,
          Rqst->OPCODE->strct.Command);
      if (item == NULL) {
        re = snf_cmd_invalid_unregistred(Rqst);
      } else if (item->func == NULL) {
        re = snf_cmd_invalid_unimplemented(Rqst);
      } else {
        re = item->func(Rqst);
      }
    }
    snf_request_free(Rqst);
    snf_request_send(Client, re);
  }
  SNF_CLT_HANDLERS *clt_han =
      snf_var_get(SNF_VAR_CLTS_HANDLERS, SNF_CLT_HANDLERS);
  if (clt_han != NULL && clt_han->on_accept != NULL)
    clt_han->on_accept(Client);
}

void snf_request_send(SNF_CLT *Client, SNF_RQST *Request) {
  /**
   * Preparing Sending:
   * Getting Arguments Size
   */
  uint32_t Size = 0;
  SNF_RQST_ARG *args = Request->args;
  while (args) {
    Size += strlen(args->arg);
    args = args->next;
    if (args != NULL) {
      Size++;
    }
  }
  char *MsgSize;
  MsgSize = snf_uint32_to_bytes(Size, 4);
  /**
   *  Sending OPCODE
   */
  snf_snd(Client, (char *)(Request->OPCODE->opcode), 4);
  /**
   * Sending Request UID
   */
  snf_snd(Client, Request->UID, strlen(NULLREQUEST) + 1);
  /**
   * Sending Arguments Length
   */
  snf_snd(Client, MsgSize, 4);
  free(MsgSize);
  /**
   * Sending Arguments
   */
  if (Size > 0) {
    args = Request->args;
    while (args) {
      snf_snd(Client, args->arg, strlen(args->arg));
      args = args->next;
      if (args)
        snf_snd(Client, UNIT_SCEPARATOR, 1);
    }
  }
  snf_request_free(Request);
  return;
}
void snf_request_send_confirm(SNF_CLT *Client, SNF_RQST *Original) {
  snf_request_send(
      Client, snf_request_genu_base(Original, SNF_OPCODE_BASE_CMD_CONFIRM));
}
void snf_request_send_reject(SNF_CLT *Client, SNF_RQST *Original) {
  snf_request_send(Client,
                   snf_request_genu_base(Original, SNF_OPCODE_BASE_CMD_REJECT));
}
void snf_request_send_invalid(SNF_CLT *Client, SNF_RQST *Original) {
  snf_request_send(
      Client, snf_request_genu_base(Original, SNF_OPCODE_BASE_CMD_INVALID));
}
