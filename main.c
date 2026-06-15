#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define LISTEN_BACKLOG 50

int main(int argc, char *argv[])
{
    int fd;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_size = sizeof(client_address);

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9632);
    server_address.sin_addr.s_addr = INADDR_ANY;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    bind(fd, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(fd, LISTEN_BACKLOG);

    while (1)
    {
        int client_fd = accept(fd, (struct sockaddr *)&client_address, &client_address_size);
        close(client_fd);
    }

    close(fd);
    return 0;
}