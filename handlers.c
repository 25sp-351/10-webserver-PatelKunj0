#include "handlers.h"
#include "response.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define HANDLER_BUFFER 4096

void handle_static(int socket_fd, const char *request_uri) {
    const char *relative_path = request_uri + strlen("/static");
    if (strstr(relative_path, "..") != NULL) {
        send_error(socket_fd, 400, "Bad Request");
        return;
    }

    char file_path[1024];
    snprintf(file_path, sizeof(file_path), "static%s", relative_path);

    int file_descriptor = open(file_path, O_RDONLY);
    if (file_descriptor < 0) {
        send_error(socket_fd, 404, "Not Found");
        return;
    }

    struct stat file_stat;
    fstat(file_descriptor, &file_stat);
    const char *mime_type = get_mime_type(file_path);

    char header[256];
    int  header_length = snprintf(
        header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %jd\r\n"
        "Connection: close\r\n"
        "\r\n",
        mime_type,
        (intmax_t)file_stat.st_size);

    send_all(socket_fd, header, header_length);

    char buffer[HANDLER_BUFFER];
    ssize_t bytes_read;
    while ((bytes_read = read(file_descriptor, buffer, sizeof(buffer))) > 0) {
        send_all(socket_fd, buffer, (size_t)bytes_read);
    }

    close(file_descriptor);
}

void handle_calc(int socket_fd, const char *request_uri) {
    char uri_copy[1024];
    strncpy(uri_copy, request_uri, sizeof(uri_copy));

    char *parts[5] = {0};
    int   index = 0;
    char *token = strtok(uri_copy, "/");

    while (token != NULL && index < 5) {
        parts[index++] = token;
        token = strtok(NULL, "/");
    }

    if (index != 4) {
        send_error(socket_fd, 400, "Bad Request");
        return;
    }

    const char *operation = parts[1];
    double      value_one  = atof(parts[2]);
    double      value_two  = atof(parts[3]);
    double      result;

    if (strcmp(operation, "add") == 0) {
        result = value_one + value_two;
    } else if (strcmp(operation, "mul") == 0) {
        result = value_one * value_two;
    } else if (strcmp(operation, "div") == 0) {
        if (value_two == 0.0) {
            send_error(socket_fd, 400, "Division by zero");
            return;
        }
        result = value_one / value_two;
    } else {
        send_error(socket_fd, 404, "Not Found");
        return;
    }

    char body[256];
    int  body_length = snprintf(
        body, sizeof(body),
        "<html><body>%.2f %s %.2f = %.2f</body></html>\n",
        value_one,
        (strcmp(operation, "add") == 0 ? "+" :
         strcmp(operation, "mul") == 0 ? "*" : "/"),
        value_two,
        result);

    char header[256];
    int  header_length = snprintf(
        header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n",
        body_length);

    send_all(socket_fd, header, header_length);
    send_all(socket_fd, body, body_length);
}