#pragma once

#define NO_SYS 1
#define LWIP_SOCKET 0
#define LWIP_NETCONN 0
#define MEM_ALIGNMENT 4
#define MEM_SIZE 1600
#define MEMP_NUM_PBUF 16
#define PBUF_POOL_SIZE 8
#define MEMP_NUM_TCP_PCB 4
#define MEMP_NUM_TCP_SEG 16
#define TCP_QUEUE_OOSEQ 0
#define TCP_MSS 1460
#define TCP_SND_BUF (2*TCP_MSS)
#define TCP_WND (2*TCP_MSS)
#define LWIP_NETIF_HOSTNAME 1