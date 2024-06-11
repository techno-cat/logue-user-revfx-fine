/*
Copyright 2024 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#ifdef __cplusplus
extern "C" {
#endif

#define LCW_REVERB_PRE_SIZE (1<<12)
#define LCW_REVERB_PRE_MAX (1)
#define LCW_REVERB_PRE_BUFFER_TOTAL (LCW_REVERB_PRE_SIZE * LCW_REVERB_PRE_MAX)

#define LCW_REVERB_COMB_SIZE (1<<12)
#define LCW_REVERB_COMB_MAX (8)
#define LCW_REVERB_COMB_BUFFER_TOTAL (LCW_REVERB_COMB_SIZE * LCW_REVERB_COMB_MAX)

#define LCW_REVERB_AP_SIZE (1<<12)
#define LCW_REVERB_AP_MAX (4)
#define LCW_REVERB_AP_BUFFER_TOTAL (LCW_REVERB_AP_SIZE * LCW_REVERB_AP_MAX)

#ifdef __cplusplus
}
#endif
