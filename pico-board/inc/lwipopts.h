#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

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

// memory pools
#define MEM_ALIGNMENT            4
#define MEM_SIZE                 4096
#define MEMP_NUM_PBUF            16
#define PBUF_POOL_SIZE           16
#define PBUF_POOL_BUFSIZE        512

// avoid redefinition of struct timeval in sockets.h
#define LWIP_TIMEVAL_PRIVATE     0
#include <sys/time.h>

#endif /* __LWIPOPTS_H__ */