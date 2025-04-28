#include "request.h"
#include "response.h"
#include "hardware/gpio.h"
#include <string.h>

#define LED_PIN 18 // This maps to GPIO 18

static void serve_root(struct tcp_pcb* pcb){
    const char* response =  "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/plain\r\n"
                            "Content-Length: 17\r\n"
                            "\r\n"
                            "hello from pico!\n";
    tcp_write(pcb, response, strlen(response), TCP_WRITE_FLAG_COPY);
}

static void serve_led_toggle(struct tcp_pcb *pcb){
    static bool led_state = false;
    led_state = !led_state;
    gpio_put(LED_PIN, led_state);

    const char *on_msg =    "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/plain\r\n"
                            "Content-Length: 12\r\n"
                            "\r\n"
                            "LED ON\n";

    const char *off_msg =   "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/plain\r\n"
                            "Content-Length: 13\r\n"
                            "\r\n"
                            "LED OFF\n";

    const char *response = led_state ? on_msg : off_msg;
    tcp_write(pcb, response, strlen(response), TCP_WRITE_FLAG_COPY);
}

void handle_http_request(struct tcp_pcb *pcb, const char *data, size_t len){
    if(strncmp(data, "GET / ", 6) == 0) 
    {
        serve_root(pcb);
    } else if (strncmp(data, "GET /index.html", 15) == 0)
    {
        send_http_response(pcb, "/public/index.html");
    }else if (strncmp(data, "GET /led", 8) == 0) {
        serve_led_toggle(pcb);
    } else{
        send_404_response(pcb);
    }
    
}