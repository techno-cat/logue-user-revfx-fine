/*
Copyright 2024 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include <stdint.h>
#include "LCWDelay.h"
#include "LCWReverbConfig.h"
#include "LCWFilterIir.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    LCWFilterIir2 lpf, hpf[2];
    LCWFilterIir1 combLpf[LCW_REVERB_COMB_MAX];
    LCWDelayBuffer preBuffer;
    LCWDelayBuffer combBuffers;
    LCWDelayBuffer apBuffers[LCW_REVERB_AP_MAX];
    // parameter
    int32_t preDelaySize;
    float combFbGain[LCW_REVERB_COMB_MAX];
    int32_t combDelaySize[LCW_REVERB_COMB_MAX];
    float apFbGain[LCW_REVERB_AP_MAX];
    int32_t apDelaySize[LCW_REVERB_AP_MAX];
} LCWReverbBlock;

extern void LCWInitPreBuffer(LCWReverbBlock *block, float *buffer);
extern void LCWInitCombBuffer(LCWReverbBlock *block, float *buffer);
extern void LCWInitApBuffer(LCWReverbBlock *block, float *buffer);

extern void LCWInputPreBuffer(float *out, const float *in, LCWReverbBlock *block);
extern void LCWInputCombLines(float *outL, float *outR, float in, LCWReverbBlock *block);
extern float LCWInputAllPassL(float in, LCWReverbBlock *block);
extern float LCWInputAllPassR(float in, LCWReverbBlock *block);

#ifdef __cplusplus
}
#endif