#ifndef locate3_h
#define locate3_h
#include <stdint.h>
#include <stdio.h>

extern uint32_t hashlittle(const void *key, size_t length, uint32_t initval);
extern void hashlittle2(const void *key, size_t length, uint32_t *pc, uint32_t *pb);
extern uint32_t hashbig( const void *key, size_t length, uint32_t initval);
#endif