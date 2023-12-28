#include "_Imports.h"
#include "sqlite.h"

sqlite3 *db;
char *SQLiteDatabase;

void generate_UUID_NotTaken(char uuid[UUID_STR_LEN])
{
    int res;
    uuid_t b;
    do
    {
        uuid_generate_time_safe(b);
        uuid_unparse_upper(b, uuid);
        res = clt_check_uuid(uuid);
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
    sqlite3_stmt *sqlstt;
    SQLiteDatabase = malloc(strlen(DB_PATH) + 3 * sizeof(char));
    sprintf(SQLiteDatabase, "%s.db", DB_PATH);
    if (local_db_connect() != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database%s: %s\n", SQLiteDatabase, sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    else
        printf("Database Conencted\n");

    if (sqlite3_prepare_v2(db, SQL_Create_Tables, -1, &sqlstt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Cannot Prepare Statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    sqlite3_step(sqlstt);
}

#define SQL_Create_Client "INSERT INTO client (uuid, sock) VALUES ( ?1 , ?2);"
void local_db_insert_clt(clt *client)
{
    int res;
    sqlite3_stmt *sqlstt;
    generate_UUID_NotTaken((client->UUID));

    if (sqlite3_prepare_v2(db, SQL_Create_Client, -1, &sqlstt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Cannot Prepare Statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    sqlite3_bind_text(sqlstt, 1, client->UUID, -1, NULL);
    sqlite3_bind_int(sqlstt, 2, client->sock);
    sqlite3_step(sqlstt);
}

#define SQL_Fetch_Client "SELECT * FROM client WHERE ?1=?2; "
clt *local_db_fetch_clt(VarType TypeFetch, void *Value)
{

    int res;
    sqlite3_stmt *sqlstt;
    uuid_t b;

    if (sqlite3_prepare_v2(db, SQL_Fetch_Client, -1, &sqlstt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Cannot Prepare Statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    if (TypeFetch == _ACTION_VAR_UUID)
    {
        sqlite3_bind_text(sqlstt, 1, "uuid", -1, NULL);
        sqlite3_bind_text(sqlstt, 2, (char *)Value, -1, NULL);
    }
    else if (TypeFetch == _ACTION_VAR_SOCK)
    {
        sqlite3_bind_text(sqlstt, 1, "sock", -1, NULL);
        sqlite3_bind_int(sqlstt, 2, *(int *)Value);
    }
    else
    {
        return NULL;
    }
    sqlite3_step(sqlstt);
    if (sqlite3_step(sqlstt) == SQLITE_ROW)
    {
        clt* Client = clt_new(sqlite3_column_int(sqlstt, 1));
        memcpy(Client->UUID, sqlite3_column_text(sqlstt, 0), 37);
        return Client;
    }
    return NULL;
}

#define SQL_Check_CLIENT " \
    SELECT \
    CASE WHEN EXISTS \
    ( \
        SELECT * FROM client \
        WHERE \
        	?1=?2 \
    ) \
    THEN 1 \
    ELSE 0 \
    END; \
    "
int local_db_check_clt(VarType TypeFetch, void *Value)
{
    int res;
    sqlite3_stmt *sqlstt;
    if (sqlite3_prepare_v2(db, SQL_Check_CLIENT, -1, &sqlstt, NULL) != SQLITE_OK)
        {
            fprintf(stderr, "Cannot Prepare Statement: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            exit(EXIT_FAILURE);
        }

    if (TypeFetch == _ACTION_VAR_UUID)
    {
        sqlite3_bind_text(sqlstt, 1, "uuid", -1, NULL);
        sqlite3_bind_text(sqlstt, 2, (char *)Value, -1, NULL);
    }
    else if (TypeFetch == _ACTION_VAR_SOCK)
    {
        sqlite3_bind_text(sqlstt, 1, "sock", -1, NULL);
        sqlite3_bind_int(sqlstt, 2, *(int *)Value);
    }
    else
        return -1;

    sqlite3_step(sqlstt);
    if (sqlite3_step(sqlstt) == SQLITE_ROW)
        return sqlite3_column_int(sqlstt, 0);

    return -1;
}