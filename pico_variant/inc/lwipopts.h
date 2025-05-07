#pragma once

#ifndef _LWIPOPTS_H
#define _LWIPOPTS_H
// Core system mode
#define NO_SYS                      1
#define LWIP_SOCKET                 0
#define LWIP_NETCONN                0

// Memory management
#define MEM_ALIGNMENT               4
#define MEM_SIZE                    4096
#define MEMP_NUM_PBUF               16
#define PBUF_POOL_SIZE              8
#define MEMP_NUM_TCP_PCB            4
#define MEMP_NUM_TCP_SEG            16
#define MEMP_NUM_SYS_TIMEOUT        8

// TCP settings for HTTP
#define TCP_QUEUE_OOSEQ             0
#define TCP_MSS                     1460
#define TCP_SND_BUF                 (2 * TCP_MSS)
#define TCP_WND                     (2 * TCP_MSS)
#define LWIP_TCP                    1

// Network interface
#define LWIP_NETIF_HOSTNAME         1
#define LWIP_NETIF_STATUS_CALLBACK  1
#define LWIP_NETIF_LINK_CALLBACK    1
#define LWIP_NETIF_EXT_STATUS_CALLBACK 0

// Protocols
#define LWIP_IPV4                   1
#define LWIP_IPV6                   0
#define LWIP_DHCP                   1
#define LWIP_DNS                    1
#define LWIP_AUTOIP                 0
#endif