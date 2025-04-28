#ifndef RESPONSE_H
#define RESPONSE_H

#include <stddef.h>


int send_all(int socket_fd, const void *data_buffer, size_t length);

const char *get_mime_type(const char *file_path);

void send_error(int socket_fd, int status_code, const char *reason_phrase);

#endif