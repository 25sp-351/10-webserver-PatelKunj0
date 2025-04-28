#include "dispatch.h"
#include "handlers.h"
#include "response.h"
#include <string.h>

void dispatch(int socket_fd, const RequestData *request) {
    if (strcmp(request->method, "GET") != 0) {
        send_error(socket_fd, 405, "Method Not Allowed");
    } 
    else if (strncmp(request->path, "/static/", 8) == 0) {
        handle_static(socket_fd, request->path);
    } 
    else if (strncmp(request->path, "/calc/", 6) == 0) {
        handle_calc(socket_fd, request->path);
    } 
    else {
        send_error(socket_fd, 404, "Not Found");
    }
}