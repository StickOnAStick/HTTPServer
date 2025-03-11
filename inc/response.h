#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void generate_http_response(int client_socket, const char* body);

#endif