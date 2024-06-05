/*
Copyright 2024 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LCW_DELAY_BUFFER_DEC(p) ( ((p)->pointer - 1) & (p)->mask )
#define LCW_DELAY_BUFFER_LUT(p, i) ( (p)->buffer[((p)->pointer + (i)) & (p)->mask] )

typedef struct {
    float *buffer;
    uint32_t size;
    uint32_t mask;
    int32_t pointer;
} LCWDelayBuffer;

#ifdef __cplusplus
}
#endif
