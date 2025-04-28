#include "request.h"
#include "dispatch.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define CONNECTION_BACKLOG 10


static void *connection_handler(void *socket_arg) {
    int client_socket_fd = *(int *)socket_arg;
    free(socket_arg);

    RequestData request;
    if (parse_request(client_socket_fd, &request) == 0) {
        dispatch(client_socket_fd, &request);
    }

    close(client_socket_fd);
    return NULL;
}

int main(int argc, char **argv) {
    int port_number = 80;
    int option;

    while ((option = getopt(argc, argv, "p:")) != -1) {
        if (option == 'p') {
            port_number = atoi(optarg);
        }
    }

    int listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket_fd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    int reuse_flag = 1;
    setsockopt(listen_socket_fd,
               SOL_SOCKET,
               SO_REUSEADDR,
               &reuse_flag,
               sizeof(reuse_flag));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(port_number);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_socket_fd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("bind");
        return EXIT_FAILURE;
    }

    if (listen(listen_socket_fd, CONNECTION_BACKLOG) < 0) {
        perror("listen");
        return EXIT_FAILURE;
    }

    printf("Server listening on port %d...\n", port_number);

    while (1) {
        int *client_sock_ptr = malloc(sizeof(int));
        *client_sock_ptr = accept(listen_socket_fd, NULL, NULL);

        if (*client_sock_ptr < 0) {
            perror("accept");
            free(client_sock_ptr);
            continue;
        }

        pthread_t thread_id;
        pthread_create(&thread_id,
                       NULL,
                       connection_handler,
                       client_sock_ptr);
        pthread_detach(thread_id);
    }

    close(listen_socket_fd);
    return EXIT_SUCCESS;
}