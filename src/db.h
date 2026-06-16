#ifndef DB_H
#define DB_H

#include <libpq-fe.h>

void load_env(void);

PGconn *db_connect(void);

void db_seed();

PGresult *db_get_user(PGconn *conn, const char *username, const char *password);

#endif