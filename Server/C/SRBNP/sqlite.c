#include "_Imports.h"
#include "sqlite.h"

char *SQLiteDatabase;
pthread_mutex_t _DB_MUTEX;

void generate_UUID_NotTaken(char uuid[UUID_STR_LEN])
{
    int res = 1;
    uuid_t b;
    do
    {
        uuid_generate_time_safe(b);
        uuid_unparse_upper(b, uuid);
        // res = clt_check_uuid(uuid); // TODO: local_db_check_clt needs work
    } while (!res);
}

#define SQL_Create_Tables "\
CREATE TABLE IF NOT EXISTS client \
(	\
    UUID varchar(37) UNIQUE, \
    sock INT, \
    PRIMARY KEY(UUID) \
);"
void local_db_inis(const char *DB_PATH)
{
    sqlite3 *db;
    sqlite3_stmt *sqlstt;
    SQLiteDatabase = malloc(strlen(DB_PATH) + 4 * sizeof(char));
    sprintf(SQLiteDatabase, "%s.db", DB_PATH);

    if (local_db_connect() != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database%s: %s\n", "{Inis}", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    else
        printf("Database Conencted\n");

    if (sqlite3_prepare_v2(db, SQL_Create_Tables, -1, &sqlstt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Cannot Prepare Statement ( Create Tables ) : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    if (sqlite3_step(sqlstt) != SQLITE_DONE)
    {
        fprintf(stderr, "Error while Intializing Database: %s\n", sqlite3_errmsg(db));
        sqlite3_close_v2(db);
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(sqlstt);
    if (sqlite3_close(db) != SQLITE_OK)
    {
        fprintf(stderr, "Error while Closing DB after Intitalizing Database: %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(&_DB_MUTEX, NULL);
}

#define SQL_Create_Client "INSERT INTO client (uuid, sock) VALUES ( ?1 , ?2);"
void local_db_insert_clt(Clt *client)
{
    int res;
    sqlite3_stmt *sqlstt;
    generate_UUID_NotTaken((client->UUID));
    sqlite3 *db;
    pthread_mutex_lock(&_DB_MUTEX);
    if (local_db_connect() != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database%s: %s\n", "{Insert Clt}", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    if (sqlite3_prepare_v2(db, SQL_Create_Client, -1, &sqlstt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Cannot Prepare Statement ( Insert Client ) : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    sqlite3_bind_text(sqlstt, 1, client->UUID, -1, NULL);
    sqlite3_bind_int(sqlstt, 2, client->sock);

    if (sqlite3_step(sqlstt) != SQLITE_DONE)
    {
        fprintf(stderr, "Error while inserting Client: %s\n", sqlite3_errmsg(db));
        fprintf(stderr, "Client UUID : %s | sock: %d\n", client->UUID, client->sock);
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    sqlite3_finalize(sqlstt);
    if (sqlite3_close(db) != SQLITE_OK)
    {
        fprintf(stderr, "Error while Closing inserting Client: %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    pthread_mutex_unlock(&_DB_MUTEX);
}

#define SQL_Fetch_Client_UUID "SELECT * FROM client WHERE uuid=?1; "
#define SQL_Fetch_Client_SOCK "SELECT * FROM client WHERE sock=?1; "
Clt *local_db_fetch_clt(VarType TypeFetch, void *Value)
{
    int res;
    sqlite3 *db;
    sqlite3_stmt *sqlstt;
    uuid_t b;
    pthread_mutex_lock(&_DB_MUTEX);
    if (local_db_connect() != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database%s: %s\n", "{Fetch Client}", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    if (sqlite3_prepare_v2(db, TypeFetch == _ACTION_VAR_SOCK ? SQL_Fetch_Client_SOCK : SQL_Fetch_Client_UUID, -1, &sqlstt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Cannot Prepare Statement ( Fetch Client ) : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    if (TypeFetch == _ACTION_VAR_UUID)
        sqlite3_bind_text(sqlstt, 1, (char *)Value, -1, NULL);
    else if (TypeFetch == _ACTION_VAR_SOCK)
        sqlite3_bind_int(sqlstt, 1, *(int *)Value);

    Clt *Client = NULL;
    if (sqlite3_step(sqlstt) == SQLITE_ROW)
    {
        Client = clt_new(sqlite3_column_int(sqlstt, 1));
        memcpy(Client->UUID, sqlite3_column_text(sqlstt, 0), 37);
    }
    sqlite3_finalize(sqlstt);
    if (sqlite3_close(db) != SQLITE_OK)
    {
        fprintf(stderr, "Error while Closing DB after Fetching Client: %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    pthread_mutex_unlock(&_DB_MUTEX);
    return Client;
}

#define SQL_Check_CLIENT_UUID "SELECT CASE WHEN EXISTS ( SELECT sock FROM client WHERE uuid=?1 ) THEN 1 ELSE 0 END;"
#define SQL_Check_CLIENT_SOCK "SELECT CASE WHEN EXISTS ( SELECT sock FROM client WHERE sock=?1 ) THEN 1 ELSE 0 END;"
int local_db_check_clt(VarType TypeFetch, void *Value)
{
    int res;
    sqlite3 *db;
    sqlite3_stmt *sqlstt;
    pthread_mutex_lock(&_DB_MUTEX);
    if (local_db_connect() != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database%s: %s\n", "{Check Client}", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    if (TypeFetch == _ACTION_VAR_UUID)
    {
        if (sqlite3_prepare_v2(db, SQL_Check_CLIENT_UUID, -1, &sqlstt, NULL) != SQLITE_OK)
        {
            fprintf(stderr, "Cannot Prepare Statement ( Check Client UUID ) : %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            exit(EXIT_FAILURE);
        }
        sqlite3_bind_text(sqlstt, 1, "uuid", -1, NULL);
        sqlite3_bind_text(sqlstt, 2, (char *)Value, -1, NULL);
    }
    else if (TypeFetch == _ACTION_VAR_SOCK)
    {
        if (sqlite3_prepare_v2(db, SQL_Check_CLIENT_SOCK, -1, &sqlstt, NULL) != SQLITE_OK)
        {
            fprintf(stderr, "Cannot Prepare Statement ( Check Client SOCK ) : %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            exit(EXIT_FAILURE);
        }
        sqlite3_bind_text(sqlstt, 1, "sock", -1, NULL);
        sqlite3_bind_int(sqlstt, 2, *(int *)Value);
    }

    int re = -1;
    if (sqlite3_step(sqlstt) == SQLITE_ROW)
        re = sqlite3_column_int(sqlstt, 0);
    else
    {
        fprintf(stderr, "Error while updating Client: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    sqlite3_finalize(sqlstt);
    if (sqlite3_close(db) != SQLITE_OK)
    {
        fprintf(stderr, "Error while Closing DB after Checking Client: %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    pthread_mutex_unlock(&_DB_MUTEX);
    return re;
}
#define SQL_Update_Client_UUID_UUID "UPDATE client SET uuid=?1 WHERE uuid=?2 ;"
#define SQL_Update_Client_UUID_SOCK "UPDATE client SET uuid=?1 WHERE sock=?2 ;"
#define SQL_Update_Client_SOCK_UUID "UPDATE client SET sock=?1 WHERE uuid=?2 ;"
#define SQL_Update_Client_SOCK_SOCK "UPDATE client SET sock=?1 WHERE sock=?2 ; commit;"
int local_db_update_clt(VarType VarToUpdate, void *NewValue, VarType VarUpdateWith, void *UpdateWithValue)
{
    int res;
    sqlite3 *db;
    sqlite3_stmt *sqlstt;
    pthread_mutex_lock(&_DB_MUTEX);
    if (local_db_connect() != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database%s: %s\n", SQLiteDatabase, sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    switch (VarToUpdate)
    {
    case _ACTION_VAR_UUID:
        switch (VarUpdateWith)
        {
        case _ACTION_VAR_UUID:
            if (sqlite3_prepare_v2(db, SQL_Update_Client_UUID_UUID, -1, &sqlstt, NULL) != SQLITE_OK)
            {
                fprintf(stderr, "Cannot Prepare Statement ( Update Client UUID _ UUID ) : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                exit(EXIT_FAILURE);
            }
            sqlite3_bind_text(sqlstt, 2, (char *)UpdateWithValue, -1, NULL);
            break;
        case _ACTION_VAR_SOCK:
            if (sqlite3_prepare_v2(db, SQL_Update_Client_UUID_SOCK, -1, &sqlstt, NULL) != SQLITE_OK)
            {
                fprintf(stderr, "Cannot Prepare Statement ( Update Client UUID _ SOCK ) : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                exit(EXIT_FAILURE);
            }
            sqlite3_bind_int(sqlstt, 2, *(int *)UpdateWithValue);
            break;
        }
        sqlite3_bind_text(sqlstt, 1, (char *)NewValue, -1, NULL);
        break;
    case _ACTION_VAR_SOCK:
        switch (VarUpdateWith)
        {
        case _ACTION_VAR_UUID:
            if (sqlite3_prepare_v2(db, SQL_Update_Client_SOCK_UUID, -1, &sqlstt, NULL) != SQLITE_OK)
            {
                fprintf(stderr, "Cannot Prepare Statement ( Update Client SOCK _ UUID ) : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                exit(EXIT_FAILURE);
            }
            sqlite3_bind_text(sqlstt, 2, (char *)UpdateWithValue, 0, NULL);
            break;
        case _ACTION_VAR_SOCK:
            if (sqlite3_prepare_v2(db, SQL_Update_Client_SOCK_SOCK, -1, &sqlstt, NULL) != SQLITE_OK)
            {
                fprintf(stderr, "Cannot Prepare Statement ( Update Client SOCK _ SOCK ) : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                exit(EXIT_FAILURE);
            }
            sqlite3_bind_int(sqlstt, 2, *(int *)UpdateWithValue);
            break;
        }
        sqlite3_bind_int(sqlstt, 1, *(int *)NewValue);
        break;
    }
    if (sqlite3_step(sqlstt) != SQLITE_DONE)
    {
        fprintf(stderr, "Error while updating Client: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    sqlite3_finalize(sqlstt);
    if (sqlite3_close(db) != SQLITE_OK)
    {
        fprintf(stderr, "Error while Closing DB after Updating Client: %s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    pthread_mutex_unlock(&_DB_MUTEX);
    return 0;
}