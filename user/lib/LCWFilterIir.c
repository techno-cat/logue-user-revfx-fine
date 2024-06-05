/*
Copyright 2022 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include "LCWFilterIir.h"

float iir1_input(LCWFilterIir1 *iir, float in)
{
  const float z1 = iir->z1;

  const float in2 = ( in - (iir->a1 * z1) );
  const float out = ( (in2 * iir->b0) + (z1 * iir->b1) );

  iir->z1 = in2;

  return out;
}

// b0とb1が同じ値を想定
float iir1_input_opt(LCWFilterIir1 *iir, float in)
{
  const float z1 = iir->z1;

  const float in2 = ( in - (iir->a1 * z1) );
  // const float out = ( (in2 * iir->b0) + (z1 * iir->b1) );
  const float out = ( (in2 + z1) * iir->b0  );

  iir->z1 = in2;

  return out;
}

float iir2_input(LCWFilterIir2 *iir, float in)
{
  const float z1 = iir->z1;
  const float z2 = iir->z2;

  const float in2 = ( in - (iir->a1 * z1) - (iir->a2 * z2) );
  const float out = ( (iir->b0 * in2) + (iir->b1 * z1) + (iir->b2 * z2) );

  iir->z2 = z1;
  iir->z1 = in2;

  return out;
}

// Q = 1/sqrt(2)を想定
float iir2_input_opt(LCWFilterIir2 *iir, float in)
{
  const float z1 = iir->z1;
  const float z2 = iir->z2;

  const float in2 = ( in - (iir->a1 * z1) - (iir->a2 * z2) );
  // const float out = ( (iir->b0 * in2) + (iir->b1 * z1) + (iir->b2 * z2) );
  const float out = ( (iir->b0 * (in2 + z2)) + (iir->b1 * z1) );

  iir->z2 = z1;
  iir->z1 = in2;

  return out;
}
