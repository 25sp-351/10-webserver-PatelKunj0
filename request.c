#include "request.h"
#include <string.h>
#include <stdio.h>

#define REQUEST_LINE_BUFFER 4096

ssize_t read_line(int socket_fd, char *buffer, size_t max_length) {
    ssize_t total_bytes = 0;
    char    current_char;

    while (total_bytes + 1 < (ssize_t)max_length) {
        ssize_t bytes_read = read(socket_fd, &current_char, 1);
        if (bytes_read <= 0) {
            return (bytes_read < 0) ? -1 : total_bytes;
        }
        if (current_char == '\r') {
            continue;
        }
        if (current_char == '\n') {
            break;
        }
        buffer[total_bytes++] = current_char;
    }

    buffer[total_bytes] = '\0';
    return total_bytes;
}

int parse_request(int socket_fd, RequestData *request) {
    char request_line[REQUEST_LINE_BUFFER];
    ssize_t line_length = read_line(socket_fd, request_line, sizeof(request_line));
    if (line_length <= 0) {
        return -1;
    }

    while (1) {
        char header_line[REQUEST_LINE_BUFFER];
        ssize_t header_length = read_line(socket_fd, header_line, sizeof(header_line));
        if (header_length < 0) {
            return -1;
        }
        if (header_length == 0) {
            break;
        }
    }

    char *method_token  = strtok(request_line, " ");
    char *path_token    = strtok(NULL, " ");
    char *version_token = strtok(NULL, " ");

    if (method_token == NULL || path_token == NULL || version_token == NULL) {
        return -1;
    }
    if (strcmp(version_token, "HTTP/1.1") != 0) {
        return -1;
    }

    snprintf(request->method, sizeof(request->method), "%s", method_token);
    snprintf(request->path,   sizeof(request->path),   "%s", path_token);

    return 0;
}