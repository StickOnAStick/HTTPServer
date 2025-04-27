#ifndef HEADER_H
#define HEADER_H

typedef struct {
    char key[64];       // Arbitrary amount of keys
    char value[256];    // Arbitrary value size 
} HttpHeader;

#endif
