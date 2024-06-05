/*
Copyright 2024 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include "userrevfx.h"
#include "buffer_ops.h"
#include "LCWReverb.h"
#include "LCWReverbParam.h"

#define param_10bit_to_6bit(val) (val >> 4) // 0..0x3FF -> 0..0x3F

static __sdram float s_reverb_ram_pre_buffer[LCW_REVERB_PRE_BUFFER_TOTAL];
static __sdram float s_reverb_ram_comb_buffer[LCW_REVERB_COMB_BUFFER_TOTAL];
static __sdram float s_reverb_ram_ap_buffer[LCW_REVERB_AP_BUFFER_TOTAL];

static struct {
    int32_t time = 0;
    float depth = 0.f;
    float mix = 0.f;
} s_param;

static float s_inputGain;
static LCWReverbBlock reverbBlock;

__fast_inline float softclip(float x)
{
    const float pre = 1.f/4.f;
    const float post = 4.f;

    return fx_softclipf(1.f/3.f, x * pre) * post;
}

void REVFX_INIT(uint32_t platform, uint32_t api)
{
    // (953) // = 48000 * 0.020
    // (331) // = 48000 * 0.0068
    // (71)  // = 48000 * 0.0015
    // (241) // = 48000 * 0.005
    // (81)  // = 48000 * 0.0017
    // (23)  // = 48000 * 0.0005
    const int32_t apDelay[] = {
        953, 241,
        331, 81,
        71, 23
    };

    s_param.depth = 0.f;
    s_param.time = 0.f;
    s_param.mix = 0.5f;
    s_inputGain = 0.f;

    LCWInitPreBuffer(&reverbBlock, s_reverb_ram_pre_buffer);
    LCWInitCombBuffer(&reverbBlock, s_reverb_ram_comb_buffer);
    LCWInitApBuffer(&reverbBlock, s_reverb_ram_ap_buffer);

    // pre-delay
    reverbBlock.preDelaySize = (48000 * 30) / 1000;

    for (int32_t i=0; i<LCW_REVERB_COMB_MAX; i++) {
        reverbBlock.combFbGain[i] = 0.f;
        reverbBlock.combDelaySize[i] = lcwCombDelaySize[i];

        LCWFilterIir1 *p = &(reverbBlock.combLpf[i]);
        const float *param = lcwCombFilterParams[i];
        p->b0 = param[0];
        p->b1 = param[1];
        p->a1 = param[2];
        p->z1 = 0.f;
    }

    {
        const int32_t n = LCW_REVERB_AP_MAX >> 1;
        for (int32_t i=0; i<n; i++) {
            // L
            reverbBlock.apFbGain[i] = 0.7f;
            reverbBlock.apDelaySize[i] = apDelay[i];
            // R
            reverbBlock.apFbGain[n+i] = 0.7f;
            reverbBlock.apDelaySize[n+i] = apDelay[i];
        }
    }

    {
        LCWFilterIir2 *p = &(reverbBlock.lpf);
        const float *param = lcwInputFilterParams[0];
        p->b0 = param[0];
        p->b1 = param[1];
        p->b2 = param[2];
        p->a1 = param[3];
        p->a2 = param[4];
        p->z1 = p->z2 = 0.f;
    }

    for (int32_t i=0; i<2; i++) {
        LCWFilterIir2 *p = &(reverbBlock.hpf[i]);
        const float *param = lcwInputFilterParams[1];
        p->b0 = param[0];
        p->b1 = param[1];
        p->b2 = param[2];
        p->a1 = param[3];
        p->a2 = param[4];
        p->z1 = p->z2 = 0.f;
    }
}

void REVFX_PROCESS(float *xn, uint32_t frames)
{
    float * __restrict x = xn;
    const float * x_e = x + 2*frames;

    const float dry = 1.f - s_param.mix;
    const float wet = s_param.mix;

    const int32_t time = param_10bit_to_6bit(s_param.time);
    for (int32_t i=0; i<LCW_REVERB_COMB_MAX; i++) {
        reverbBlock.combFbGain[i] = lcwReverbGainTable[time][i];
    }

    // 切り替え時のノイズ対策
    if ( s_inputGain < 0.99998f ) {
        for (; x != x_e; ) {
            *(x++) = *x * s_inputGain;
            *(x++) = *x * s_inputGain;

            if ( s_inputGain < 0.99998f ) {
            s_inputGain += ( (1.f - s_inputGain) * 0.0625f );
            }
            else {
                s_inputGain = 1.f;
                break;
            }
        }
    }

    const float sendLevel = s_param.depth;
    x = xn;
    for (; x != x_e; ) {
        const float xL = *(x + 0);
        const float xR = *(x + 1);

        const float tmp[] = {
            xL * sendLevel,
            xR * sendLevel
        };

        float out;
        LCWInputPreBuffer(&out, tmp, &reverbBlock);

        float combL, combR;
        LCWInputCombLines(&combL, &combR, out, &reverbBlock);

#if (1)
        const float outL =
            LCWInputAllPassL(combL * .125f, &reverbBlock);
        const float outR =
            LCWInputAllPassL(combR * .125f, &reverbBlock);
#else
        const float outL = combL * .0625f;
        const float outR = combR * .0625f;
#endif
        const float yL = softclip( (dry * xL) + (wet * outL) );
        const float yR = softclip( (dry * xR) + (wet * outR) );

        *(x++) = yL;
        *(x++) = yR;
    }
}

void REVFX_RESUME(void)
{
    buf_clr_f32(s_reverb_ram_pre_buffer, LCW_REVERB_PRE_BUFFER_TOTAL);
    buf_clr_f32(s_reverb_ram_comb_buffer, LCW_REVERB_COMB_BUFFER_TOTAL);
    buf_clr_f32(s_reverb_ram_ap_buffer, LCW_REVERB_AP_BUFFER_TOTAL);
    s_inputGain = 0.f;
}

void REVFX_PARAM(uint8_t index, int32_t value)
{
    const float valf = q31_to_f32(value);
    switch (index) {
    case k_user_revfx_param_time:
        s_param.time = clipminmaxi32(0, (int32_t)(valf * 1024.f), 1023);
        break;
    case k_user_revfx_param_depth:
        s_param.depth = clip01f(valf);
        break;
    case k_user_revfx_param_shift_depth:
        // Rescale to add notch around 0.5f
        s_param.mix = (valf <= 0.49f) ? 1.02040816326530612244f * valf : (valf >= 0.51f) ? 0.5f + 1.02f * (valf-0.51f) : 0.5f;
        break;
    default:
        break;
    }
}
