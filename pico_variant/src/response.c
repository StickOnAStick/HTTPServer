#include "response.h"
#include "pico/stdlib.h"
#include <string.h>
#include "generated_fs.h"


void send_http_response(struct tcp_pcb *pcb, const char* path){
    size_t file_size = public_home_html_len;

    char header[256];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %u\r\n"
        "\r\n", (unsigned int)file_size
    );

    tcp_write(pcb, header, header_len, TCP_WRITE_FLAG_COPY);
    tcp_write(pcb, public_home_html, file_size, TCP_WRITE_FLAG_COPY);
}

void send_404_response(struct tcp_pcb* pcb){
    const char* not_found = 
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Lenght: 13\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "404 Not Found\n";

        tcp_write(pcb, not_found, strlen(not_found), TCP_WRITE_FLAG_COPY);
}