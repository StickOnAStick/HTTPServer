#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "cyw43_arch.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"

#include "inc/request.h"
#include "inc/response.h"

#define PORT 8080
#define BUFFER_SIZE 1024 // 1kb buff
#define MAX_CONNECTIONS 5

void handle_client(struct netconn *client_conn) {
    char buffer[BUFFER_SIZE];
    int bytes_received = netconn_recv(client_conn, buffer, sizeof(buffer));
    if (bytes_received < 8) {
        printf("Error: Data too small for parsing\n");
        netconn_close(client_conn);
        return;
    }

    HttpRequest req;
    parse_http_request(buffer, &req);

    printf("Received request: %s %s %s\n", req.method, req.path, req.version);

    generate_http_response(client_conn, req.path);
    
    netconn_close(client_conn);
}

int main() {
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("Failed to initialize Wi-Fi\n");
        return 1;
    }

    // Connect to Wi-Fi
    const char *ssid = "yourSSID";
    const char *password = "yourPassword";
    if (cyw43_arch_wifi_connect(ssid, password)) {
        printf("Failed to connect to Wi-Fi\n");
        return 1;
    }

    // Start a server
    struct netconn *server_conn;
    server_conn = netconn_new(NETCONN_TCP);
    if (server_conn == NULL) {
        printf("Failed to create server socket\n");
        return 1;
    }

    netconn_bind(server_conn, NULL, PORT);
    netconn_listen(server_conn);

    printf("HTTP Server running on port %d...\n", PORT);

    while (1) {
        struct netconn *client_conn;
        if (netconn_accept(server_conn, &client_conn) == ERR_OK) {
            handle_client(client_conn);
        }
        netconn_delete(client_conn);
    }

    netconn_delete(server_conn);
    return 0;
}
