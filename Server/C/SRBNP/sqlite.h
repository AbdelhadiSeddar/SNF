#ifndef sqlite_h
#define sqlite_h
#include "../SQLite/sqlite3.h"
#include "clt.h"

extern char* SQLiteDatabase;
extern sqlite3* db;
/// @brief Connects to the local Database
/// @return SQLITE_OK -> Connected | Else Error 
#define local_db_connect() sqlite3_open( SQLiteDatabase, &db)

#define SQL_Create_Tables "\
CREATE TABLE IF NOT EXISTS client \
(	\
    UUID varchar(37) UNIQUE, \
    sock INT, \
    PRIMARY KEY(UUID) \
);"
#define SQL_Check_GUID " \
    SELECT \
    CASE WHEN EXISTS \
    ( \
        SELECT * FROM client \
        WHERE \
        	UUID=?1 \
    ) \
    THEN 1 \
    ELSE 0 \
    END; \
    "
#define SQL_Create_Client "INSERT INTO client (uuid, sock) VALUES ( ?1 , ?2);"

extern void local_db_inis();
extern void local_db_insert_clt(clt* client);

#endif