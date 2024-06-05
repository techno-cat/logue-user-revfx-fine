/*
Copyright 2024 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include <stdint.h>
#include "LCWReverbConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LCW_REVERB_GAIN_TABLE_SIZE (64)

extern const int32_t lcwCombDelaySize[LCW_REVERB_COMB_MAX];

extern const float lcwReverbGainTable[LCW_REVERB_GAIN_TABLE_SIZE][LCW_REVERB_COMB_MAX];

extern const float lcwInputFilterParams[][5];
extern const float lcwCombFilterParams[][3];

#ifdef __cplusplus
}
#endif
