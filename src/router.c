#include <stdio.h>
#include <ctype.h>

#include "router.h"
#include "db.h"

static int hexval(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

void url_decode(char *src, char *dest)
{
    char *p = src;
    char *o = dest;

    while (*p)
    {
        if (*p == '+')
        {
            *o++ = ' ';
            p++;
        }
        else if (*p == '%' && isxdigit(p[1]) && isxdigit(p[2]))
        {
            int hi = hexval(p[1]);
            int lo = hexval(p[2]);
            *o++ = (char)((hi << 4) | lo);
            p += 3;
        }
        else
        {
            *o++ = *p++;
        }
    }

    *o = '\0';
}

const char *resolve_path(const char *url_path)
{
    if (strcmp(url_path, "/") == 0)
        return "pages/index.html";
    if (strcmp(url_path, "/desk") == 0)
        return "pages/desk.html";
    if (strcmp(url_path, "/bookshelf") == 0)
        return "pages/bookshelf.html";
    if (strcmp(url_path, "/books/anagrams") == 0)
        return "pages/books/anagrams.html";
    if (strcmp(url_path, "/books/no-exit") == 0)
        return "pages/books/no-exit.html";
    if (strcmp(url_path, "/books/sql-injections") == 0)
        return "pages/books/sql-injections.html";

    return NULL;
}

size_t read_resource(const char *path, char *content, int max_size)
{
    FILE *page = fopen(path, "r");
    if (!page)
        return -1;

    size_t n = fread(content, 1, max_size, page);
    fclose(page);
    content[n] = '\0';
    return n;
}

int build_response(char *response,
                   size_t response_size,
                   const char *status,
                   const char *content_type,
                   const char *body,
                   size_t body_len)
{
    return snprintf(
        response,
        response_size,
        "HTTP/1.1 %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%.*s",
        status,
        content_type,
        body_len,
        (int)body_len,
        body);
}

void parse_login(char *line, char *user, char *pass)
{
    user[0] = '\0';
    pass[0] = '\0';

    char *amp = strchr(line, '&');
    if (amp)
        *amp = '\0';

    char *eq1 = strchr(line, '=');
    if (eq1)
    {
        *eq1 = '\0';
        if (strcmp(line, "username") == 0)
            strcpy(user, eq1 + 1);
    }

    if (amp)
    {
        char *right = amp + 1;

        char *eq2 = strchr(right, '=');
        if (eq2)
        {
            *eq2 = '\0';
            if (strcmp(right, "password") == 0)
                strcpy(pass, eq2 + 1);
        }
    }
}

int handle_login(http_request *request, char *response, int response_size)
{
    char user[256];
    char pass[256];

    parse_login(request->body, user, pass);
    url_decode(user, user);
    url_decode(pass, pass);
    printf("%s\n", pass);

    PGconn *db_conn = db_connect();
    PGresult *user_found = db_get_user(db_conn, user, pass);

    if (PQntuples(user_found) > 0)
    {
        char content[] = "Hello Garcin. Door unlocked!";
        return build_response(response, response_size,
                              "200 OK",
                              "text/html",
                              content, strlen(content));
    }
    else
    {
        return build_response(response, response_size,
                              "401 Unauthorized",
                              "text/plain",
                              "Unauthorized",
                              12);
    }

    return -1;
}

int handle_get(http_request *request, char *response, int response_size)
{
    int max_content_len = 1024;
    char content[max_content_len];
    size_t content_len;
    const char *resource_path = resolve_path(request->path);

    if (resource_path)
    {
        content_len = read_resource(resource_path, content, max_content_len);
        return build_response(response, response_size,
                              "200 OK",
                              "text/html",
                              content, content_len);
    }
    else
    {
        return build_response(response, response_size,
                              "404 Not Found",
                              "text/plain",
                              "Not Found",
                              9);
    }
}

int handle_request(http_request *request, char *response, int response_size)
{
    printf("%s\n", request->path);

    if (strcmp(request->path, "/login") == 0 && strcmp(request->method, "POST") == 0)
    {
        return handle_login(request, response, response_size);
    }
    else if (strcmp(request->method, "GET") == 0)
    {
        return handle_get(request, response, response_size);
    }
    else
    {
        return build_response(response, response_size,
                              "403 Forbidden",
                              "text/plain",
                              "Forbidden",
                              9);
    }

    return -1;
}