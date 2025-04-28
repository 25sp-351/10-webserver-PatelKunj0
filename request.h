#ifndef REQUEST_H
#define REQUEST_H

#include <unistd.h>
#include <stddef.h>


typedef struct {
    char method[8];
    char path[1024];
} RequestData;

ssize_t read_line(int socket_fd, char *buffer, size_t max_length);

int parse_request(int socket_fd, RequestData *request);

#endif