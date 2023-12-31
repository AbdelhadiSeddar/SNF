#ifndef sqlite_h
#define sqlite_h
#include "../SQLite/sqlite3.h"
#include "clt.h"

extern char* SQLiteDatabase;
extern sqlite3* db;
/// @brief Connects to the local Database
/// @return SQLITE_OK -> Connected | Else Error 
#define local_db_connect() sqlite3_open( SQLiteDatabase, &db)


extern void local_db_inis();
extern void local_db_insert_clt(Clt* client);

typedef enum _ACTION_VAR_TYPE 
{
    _ACTION_VAR_UUID,
    _ACTION_VAR_SOCK
} VarType;
extern Clt* local_db_fetch_clt(VarType TypeFetch, void* Value);
extern int local_db_check_clt(VarType TypeFetch, void* Value);
extern int local_db_update_clt(VarType VarToUpdate, void *NewValue, VarType VarUpdateWith, void *UpdateWithValue);

#endif