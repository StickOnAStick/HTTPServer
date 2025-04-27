#ifndef RESPONSE_H
#define RESPONSE_H

#include "lwip/api.h"

void generate_http_response(struct netconn *conn, const char *path);

#endif
