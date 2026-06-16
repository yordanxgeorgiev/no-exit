#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"

int read_line(int socket, char *buffer, int max)
{
    char c;
    int i = 0;

    while (i < max - 1)
    {
        int n = recv(socket, &c, 1, 0);
        if (n <= 0)
            return n;

        buffer[i++] = c;

        if (i >= 2 &&
            buffer[i - 2] == '\r' &&
            buffer[i - 1] == '\n')
        {
            buffer[i] = '\0';
            return i;
        }
    }

    buffer[i] = '\0';
    return i;
}

int http_read(int socket, http_request *result)
{
    char buffer[1024];
    memset(result, 0, sizeof(*result));
    
    // request line
    if (read_line(socket, buffer, sizeof(buffer)) <= 0)
        return -1;

    sscanf(buffer, "%7s %127s", result->method, result->path);

    int content_length = 0;

    // headers
    while (1)
    {
        if (read_line(socket, buffer, sizeof(buffer)) <= 0)
            return -1;

        if (strcmp(buffer, "\r\n") == 0 || strcmp(buffer, "\n") == 0)
            break;

        if (sscanf(buffer, "Content-Length: %d", &content_length) == 1)
        {
            result->content_length = content_length;
        }
    }

    // body
    if (result->content_length > 0)
    {
        read(socket, result->body, result->content_length);
        result->body[result->content_length] = '\0';

        printf("BODY: %s\n", result->body);
    }

    return 0;
}