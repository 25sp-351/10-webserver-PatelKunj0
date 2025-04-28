#ifndef DISPATCH_H
#define DISPATCH_H

#include "request.h"

void dispatch(int socket_fd, const RequestData *request);

#endif