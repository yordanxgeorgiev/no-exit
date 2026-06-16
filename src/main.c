#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define PORT 9632
#define LISTEN_BACKLOG 50

#include "parser.h"
#include "router.h"

int main(int argc, char *argv[])
{
    (void)argc;  // mark as unused for compiler warnings
    (void)argv;

    int fd;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_size = sizeof(client_address);
    http_request request;
    char response[1024];

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    bind(fd, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(fd, LISTEN_BACKLOG);

    while (1)
    {
        int client_fd = accept(fd, (struct sockaddr *)&client_address, &client_address_size);
        http_read(client_fd, &request);
        int response_len = handle_request(&request, response, sizeof(response));
        write(client_fd, response, response_len);
        close(client_fd);
    }

    close(fd);
    return 0;
}