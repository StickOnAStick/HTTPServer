#include "response.h"
#include <string.h>
#include "lwip/api.h"  // Add for netconn_write
#include "lwip/netbuf.h"  // Add for NETCONN_COPY

void generate_http_response(struct netconn *conn, const char *path) {
    const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nHello, Pico W!";
    netconn_write(conn, response, strlen(response), NETCONN_COPY);
}
