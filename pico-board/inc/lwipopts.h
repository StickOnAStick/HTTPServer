#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#define TCP_LISTEN_BACKLOG       1

// run in NO_SYS=1 “no-OS” mode
#define NO_SYS                   1
#define SYS_LIGHTWEIGHT_PROT     1

// only the RAW API is allowed
#define LWIP_RAW                 1
#define LWIP_NETCONN             0
#define LWIP_SOCKET              0
#define LWIP_API_MSG             0

// protocols you actually need
#define LWIP_TCP                 1
#define LWIP_DNS                 1
#define LWIP_ICMP                1
#define LWIP_DHCP                1

// memory pools
#define MEMP_NUM_DHCP            1
#define MEMP_NUM_TCP_PCB_LISTEN  64
#define MEMP_NUM_TCP_PCB         64
#define MEMP_NUM_TCPIP_MSG_API   16
#define MEMP_NUM_TCPIP_MSG_INPKT 16
#define MEM_ALIGNMENT            4
#define MEM_SIZE                 4096
#define MEMP_NUM_PBUF            32
#define PBUF_POOL_SIZE           32
#define PBUF_POOL_BUFSIZE        512
#define MEMP_NUM_SYS_TIMEOUT     12

// avoid redefinition of struct timeval in sockets.h
#define LWIP_TIMEVAL_PRIVATE     0
#include <sys/time.h>

#endif /* __LWIPOPTS_H__ */