/*
Copyright 2024 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include "LCWReverb.h"
#include "buffer_ops.h"
#include "fx_api.h"

void LCWInitPreBuffer(LCWReverbBlock *block, float *buffer)
{
    LCWDelayBuffer *p = &(block->preBuffer);
    p->buffer = buffer;
    p->size = LCW_REVERB_PRE_BUFFER_TOTAL;
    p->mask = LCW_REVERB_PRE_SIZE - 1;
    p->pointer = 0;
}

void LCWInitCombBuffer(LCWReverbBlock *block, float *buffer)
{
    LCWDelayBuffer *p = &(block->combBuffer);
    p->buffer = buffer;
    p->size = LCW_REVERB_COMB_BUFFER_TOTAL;
    p->mask = LCW_REVERB_COMB_SIZE - 1;
    p->pointer = 0;
}

void LCWInitApBuffer(LCWReverbBlock *block, float *buffer)
{
    LCWDelayBuffer *p = &(block->apBuffer);
    p->buffer = buffer;
    p->size = LCW_REVERB_AP_BUFFER_TOTAL;
    p->mask = LCW_REVERB_AP_SIZE - 1;
    p->pointer = 0;
}

void LCWInputPreBuffer(float *out, const float *in, LCWReverbBlock *block)
{
    LCWDelayBuffer *buf = &(block->preBuffer);
    buf->pointer = LCW_DELAY_BUFFER_DEC(buf);
    buf->buffer[buf->pointer] = in[0] + in[1];

    const float zn = LCW_DELAY_BUFFER_LUT(buf, block->preDelaySize);
    const float tmp = iir2_input_opt(&(block->hpf), zn);
    *out = iir2_input_opt(&(block->lpf), tmp);
}

void LCWInputCombLines(float *out, const float in, LCWReverbBlock *block)
{
    LCWDelayBuffer *buf = &(block->combBuffer);
    buf->pointer = LCW_DELAY_BUFFER_DEC(buf);

    const uint32_t mask = buf->mask;
    const int32_t pointer = buf->pointer;

    float tmp[LCW_REVERB_COMB_MAX];
    for (int32_t i=0; i<LCW_REVERB_COMB_MAX; i++) {
        float *p = buf->buffer + (LCW_REVERB_COMB_SIZE * i);
        // -1は、ディレイラインの手前にある1次のIIRを考慮した結果
        const int32_t j = pointer + block->combDelaySize[i] - 1;
        const float fbGain = block->combFbGain[i];
        LCWFilterIir1 *lpf = &(block->combLpf[i]);

        // フィードバックを加算
        const float zn = p[(uint32_t)j & mask];
        p[pointer] = iir1_input_opt(lpf, in + (zn * fbGain));

        tmp[i] = zn;
    }

    *out = tmp[0] - tmp[1] + tmp[2] - tmp[3]
         + tmp[4] - tmp[5] + tmp[6] - tmp[7];
}

float LCWInputAllPass1(float in, LCWReverbBlock *block)
{
    LCWDelayBuffer *buf = &(block->apBuffer);
    buf->pointer = LCW_DELAY_BUFFER_DEC(buf);

    const uint32_t mask = buf->mask;
    const int32_t pointer = buf->pointer;

    float out = in;
    for (int32_t i=0; i<LCW_REVERB_AP_MAX; i++) {
        const int32_t delaySize = block->apDelaySize[i];
        const float fbGain = block->apFbGain[i];

        float *p = buf->buffer + (LCW_REVERB_AP_SIZE * i);
        const float zn = p[(uint32_t)(pointer + delaySize) & mask];

        const float in2 = out - (zn * fbGain);
        p[pointer] = in2;

        out = zn + (in2 * fbGain);
    }

    return out;
}
