#ifndef HANDLERS_H
#define HANDLERS_H

#include "request.h"


void handle_static(int socket_fd, const char *request_uri);

void handle_calc(int socket_fd, const char *request_uri);

#endif