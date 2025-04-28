#include "response.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

int send_all(int socket_fd, const void *data_buffer, size_t length) {
    size_t total_sent = 0;
    while (total_sent < length) {
        ssize_t sent = write(socket_fd,
                             (const char *)data_buffer + total_sent,
                             length - total_sent);
        if (sent <= 0) {
            return -1;
        }
        total_sent += (size_t)sent;
    }
    return 0;
}

const char *get_mime_type(const char *file_path) {
    const char *extension = strrchr(file_path, '.');
    if (extension == NULL) {
        return "application/octet-stream";
    }
    if (strcmp(extension, ".html") == 0 || strcmp(extension, ".htm") == 0) {
        return "text/html";
    }
    if (strcmp(extension, ".css") == 0) {
        return "text/css";
    }
    if (strcmp(extension, ".js") == 0) {
        return "application/javascript";
    }
    if (strcmp(extension, ".png") == 0) {
        return "image/png";
    }
    if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) {
        return "image/jpeg";
    }
    if (strcmp(extension, ".gif") == 0) {
        return "image/gif";
    }
    if (strcmp(extension, ".txt") == 0) {
        return "text/plain";
    }
    return "application/octet-stream";
}

void send_error(int socket_fd, int status_code, const char *reason_phrase) {
    char body[256];
    int  body_length = snprintf(
        body, sizeof(body),
        "<html><body><h1>%d %s</h1></body></html>\n",
        status_code, reason_phrase);

    char header[256];
    int  header_length = snprintf(
        header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n",
        status_code, reason_phrase, body_length);

    send_all(socket_fd, header, header_length);
    send_all(socket_fd, body, body_length);
}