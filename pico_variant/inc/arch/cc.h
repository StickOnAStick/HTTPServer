#pragma once

// Pull correct standard types
#include <stdint.h>  // for uint8_t, uint16_t, uint32_t
#include <stddef.h>  // for size_t

// typedef for lwIP critical section protection
typedef uint32_t sys_prot_t;

// Standard platform-specific defines
#include <stdio.h>
#include <stdlib.h>

#define LWIP_PLATFORM_DIAG(x)    do { printf x; } while(0)
#define LWIP_PLATFORM_ASSERT(x)  do { printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); fflush(NULL); abort(); } while(0)

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__((__packed__))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#define LWIP_RAND() ((u32_t)rand())
