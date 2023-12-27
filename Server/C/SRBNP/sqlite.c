#include "_Imports.h"

sqlite3 *db;
char *SQLiteDatabase;

void generate_UUID_NotTaken(char uuid[UUID_STR_LEN])
{
    int res;
    sqlite3_stmt *sqlstt;
    uuid_t b;
    do
    {
        uuid_generate_time(b);
        uuid_unparse_upper(b, uuid);
        if (sqlite3_prepare_v2(db, SQL_Check_GUID, -1, &sqlstt, NULL) != SQLITE_OK)
        {
            fprintf(stderr, "Cannot Prepare Statement: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            exit(EXIT_FAILURE);
        }

        sqlite3_bind_text(sqlstt, 1, uuid, -1, NULL);
        sqlite3_step(sqlstt);
        if(sqlite3_step(sqlstt) == SQLITE_ROW)
        {
            res = sqlite3_column_int(sqlstt, 0);
        }
    } while (!res);
}

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

void local_db_insert_clt(clt* client)
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