#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <sys/socket.h>

typedef struct
{
    char method[8];
    char path[128];
    int content_length;
    char body[4096];;
} http_request;

int http_read(int socket, http_request *result);

int read_line(int socket, char *buffer, int max);

#endif