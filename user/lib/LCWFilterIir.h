/*
Copyright 2022 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float z1;
    float a1;
    float b0, b1;
} LCWFilterIir1;

typedef struct {
    float z1, z2;
    float a1, a2;
    float b0, b1, b2;
} LCWFilterIir2;

extern float iir1_input(LCWFilterIir1 *iir, float in);

// b0とb1が同じ値を想定
extern float iir1_input_opt(LCWFilterIir1 *iir, float in);

extern float iir2_input(LCWFilterIir2 *iir, float in);

// Q = 1/sqrt(2)を想定（b0とb2が同じ値）
extern float iir2_input_opt(LCWFilterIir2 *iir, float in);

#ifdef __cplusplus
}
#endif
