#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

void load_env()
{
    FILE *f = fopen(".env", "r");
    if (!f)
    {
        perror("fopen");
        return;
    }

    char line[256];

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;

        if (line[0] == '\0')
            continue;

        char *eq = strchr(line, '=');
        if (!eq)
            continue;

        *eq = '\0';

        char *key = line;
        char *value = eq + 1;

        setenv(key, value, 1);
    }

    fclose(f);
}

PGconn *db_connect(void)
{
    const char *user = getenv("POSTGRES_USER");
    const char *password = getenv("POSTGRES_PASSWORD");
    const char *db = getenv("POSTGRES_DB");
    const char *port = getenv("POSTGRES_PORT");
    const char *host = getenv("POSTGRES_HOST");

    if (!host)
        host = "localhost";
    if (!port)
        port = "5432";

    if (!user || !password || !db)
    {
        fprintf(stderr, "Missing required environment variables\n");
        return NULL;
    }

    char conninfo[512];
    snprintf(conninfo, sizeof(conninfo),
             "host=%s port=%s dbname=%s user=%s password=%s",
             host, port, db, user, password);

    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }

    printf("DB connected successfully!\n");
    return conn;
}

void db_seed()
{
    load_env();
    PGconn *conn = db_connect();

    const char *create_table =
        "CREATE TABLE IF NOT EXISTS users ("
        "id SERIAL PRIMARY KEY, "
        "username TEXT UNIQUE NOT NULL, "
        "password TEXT NOT NULL"
        ");";

    PGresult *res = PQexec(conn, create_table);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Create table failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }
    PQclear(res);

    const char *insert_sql =
        "INSERT INTO users (username, password) VALUES "
        "('Garcin', 'nicraG') "
        "ON CONFLICT (username) DO NOTHING;";

    res = PQexec(conn, insert_sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Seed insert failed: %s\n", PQerrorMessage(conn));
    }
    else
    {
        printf("Database seeded successfully.\n");
    }

    PQclear(res);
    PQfinish(conn);
}

PGresult *db_get_user(PGconn *conn, const char *username, const char *password)
{    
    char query[256];
    // SQL injectable by design ;)
    sprintf(query,
        "SELECT id FROM users WHERE username='%s' AND password='%s'",
        username, password);

    printf("%s\n", query);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Query failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    return res;
}