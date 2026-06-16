#include <stdio.h>

#include "router.h"

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

int handle_login(http_request *request, char *response, int response_size)
{
    char content[] = "ok";
    size_t content_len = 2;

    // this is just placeholder login logic
    if (strcmp(request->body, "username=a&password=b") == 0)
    {
        return build_response(response, response_size,
                              "200 OK",
                              "text/html",
                              content, content_len);
    }
    else
    {
        return build_response(response, response_size,
                              "401 Unauthorized",
                              "text/plain",
                              "Unauthorized",
                              12);
    }
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