/******************************************************************************
 *
 *  Copyright (C) 2014 The Android Open Source Project
 *  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/**
 @file

 DO NOT EDIT THIS FILE DIRECTLY

 This file is automatically generated by the "synthesis-gen.pl" script.
 Any changes to this generated file will be lost when the script is re-run.

 These functions are called by functions in synthesis.c to perform the synthesis
 filterbank computations for the SBC decoder.


 */

#include "../include/oi_codec_sbc_private.h"

#ifndef CLIP_INT16
#define CLIP_INT16(x) do { if (x > OI_INT16_MAX) { x = OI_INT16_MAX; } else if (x < OI_INT16_MIN) { x = OI_INT16_MIN; } } while (0)
#endif

#define MUL_16S_16S(_x, _y) ((_x) * (_y))

PRIVATE void SynthWindow80_generated(OI_INT16 *pcm, SBC_BUFFER_T const * RESTRICT buffer, OI_UINT strideShift);
PRIVATE void SynthWindow80_generated(OI_INT16 *pcm, SBC_BUFFER_T const * RESTRICT buffer, OI_UINT strideShift)
{
    OI_INT32 pcm_a, pcm_b;
    /* 1 - stage 0 */ pcm_b = 0;
    /* 1 - stage 0 */ pcm_b +=(MUL_16S_16S(8235, buffer[ 12]))>> 3;
    /* 1 - stage 0 */ pcm_b +=(MUL_16S_16S(-23167, buffer[ 20]))>> 3;
    /* 1 - stage 0 */ pcm_b +=(MUL_16S_16S(26479, buffer[ 28]))>> 2;
    /* 1 - stage 0 */ pcm_b +=(MUL_16S_16S(-17397, buffer[ 36]))<< 1;
    /* 1 - stage 0 */ pcm_b +=(MUL_16S_16S(9399, buffer[ 44]))<< 3;
    /* 1 - stage 0 */ pcm_b +=(MUL_16S_16S(17397, buffer[ 52]))<< 1;
    /* 1 - stage 0 */ pcm_b +=(MUL_16S_16S(26479, buffer[ 60]))>> 2;
    /* 1 - stage 0 */ pcm_b +=(MUL_16S_16S(23167, buffer[ 68]))>> 3;
    /* 1 - stage 0 */ pcm_b +=(MUL_16S_16S(8235, buffer[ 76]))>> 3;
    /* 1 - stage 0 */ pcm_b /= 32768; CLIP_INT16(pcm_b); pcm[0<<strideShift] = (OI_INT16)pcm_b;
    /* 1 - stage 1 */ pcm_a = 0;
    /* 1 - stage 1 */ pcm_b = 0;
    /* 1 - stage 1 */ pcm_a +=(MUL_16S_16S(-3263, buffer[  5]))>> 5;
    /* 1 - stage 1 */ pcm_b +=(MUL_16S_16S(9293, buffer[  5]))>> 3;
    /* 1 - stage 1 */ pcm_a +=(MUL_16S_16S(29293, buffer[ 11]))>> 5;
    /* 1 - stage 1 */ pcm_b +=(MUL_16S_16S(-6087, buffer[ 11]))>> 2;
    /* 1 - stage 1 */ pcm_a +=(MUL_16S_16S(-5229, buffer[ 21]));
    /* 1 - stage 1 */ pcm_b +=(MUL_16S_16S(1247, buffer[ 21]))<< 3;
    /* 1 - stage 1 */ pcm_a +=(MUL_16S_16S(30835, buffer[ 27]))>> 3;
    /* 1 - stage 1 */ pcm_b +=(MUL_16S_16S(-2893, buffer[ 27]))<< 3;
    /* 1 - stage 1 */ pcm_a +=(MUL_16S_16S(-27021, buffer[ 37]))<< 1;
    /* 1 - stage 1 */ pcm_b +=(MUL_16S_16S(23671, buffer[ 37]))<< 2;
    /* 1 - stage 1 */ pcm_a +=(MUL_16S_16S(31633, buffer[ 43]))<< 1;
    /* 1 - stage 1 */ pcm_b +=(MUL_16S_16S(18055, buffer[ 43]))<< 1;
    /* 1 - stage 1 */ pcm_a +=(MUL_16S_16S(17319, buffer[ 53]))<< 1;
    /* 1 - stage 1 */ pcm_b +=(MUL_16S_16S(11537, buffer[ 53]))>> 1;
    /* 1 - stage 1 */ pcm_a +=(MUL_16S_16S(26663, buffer[ 59]))>> 2;
    /* 1 - stage 1 */ pcm_b +=(MUL_16S_16S(1747, buffer[ 59]))<< 1;
    /* 1 - stage 1 */ pcm_a +=(MUL_16S_16S(4555, buffer[ 69]))>> 1;
    /* 1 - stage 1 */ pcm_b +=(MUL_16S_16S(685, buffer[ 69]))<< 1;
    /* 1 - stage 1 */ pcm_a +=(MUL_16S_16S(12419, buffer[ 75]))>> 4;
    /* 1 - stage 1 */ pcm_b +=(MUL_16S_16S(8721, buffer[ 75]))>> 7;
    /* 1 - stage 1 */ pcm_a /= 32768; CLIP_INT16(pcm_a); pcm[1<<strideShift] = (OI_INT16)pcm_a;
    /* 1 - stage 1 */ pcm_b /= 32768; CLIP_INT16(pcm_b); pcm[7<<strideShift] = (OI_INT16)pcm_b;
    /* 1 - stage 2 */ pcm_a = 0;
    /* 1 - stage 2 */ pcm_b = 0;
    /* 1 - stage 2 */ pcm_a +=(MUL_16S_16S(-10385, buffer[  6]))>> 6;
    /* 1 - stage 2 */ pcm_b +=(MUL_16S_16S(11167, buffer[  6]))>> 4;
    /* 1 - stage 2 */ pcm_a +=(MUL_16S_16S(24995, buffer[ 10]))>> 5;
    /* 1 - stage 2 */ pcm_b +=(MUL_16S_16S(-10337, buffer[ 10]))>> 4;
    /* 1 - stage 2 */ pcm_a +=(MUL_16S_16S(-309, buffer[ 22]))<< 4;
    /* 1 - stage 2 */ pcm_b +=(MUL_16S_16S(1917, buffer[ 22]))<< 2;
    /* 1 - stage 2 */ pcm_a +=(MUL_16S_16S(9161, buffer[ 26]))>> 3;
    /* 1 - stage 2 */ pcm_b +=(MUL_16S_16S(-30605, buffer[ 26]))>> 1;
    /* 1 - stage 2 */ pcm_a +=(MUL_16S_16S(-23063, buffer[ 38]))<< 1;
    /* 1 - stage 2 */ pcm_b +=(MUL_16S_16S(8317, buffer[ 38]))<< 3;
    /* 1 - stage 2 */ pcm_a +=(MUL_16S_16S(27561, buffer[ 42]))<< 1;
    /* 1 - stage 2 */ pcm_b +=(MUL_16S_16S(9553, buffer[ 42]))<< 2;
    /* 1 - stage 2 */ pcm_a +=(MUL_16S_16S(2309, buffer[ 54]))<< 3;
    /* 1 - stage 2 */ pcm_b +=(MUL_16S_16S(22117, buffer[ 54]))>> 4;
    /* 1 - stage 2 */ pcm_a +=(MUL_16S_16S(12705, buffer[ 58]))>> 1;
    /* 1 - stage 2 */ pcm_b +=(MUL_16S_16S(16383, buffer[ 58]))>> 2;
    /* 1 - stage 2 */ pcm_a +=(MUL_16S_16S(6239, buffer[ 70]))>> 3;
    /* 1 - stage 2 */ pcm_b +=(MUL_16S_16S(7543, buffer[ 70]))>> 3;
    /* 1 - stage 2 */ pcm_a +=(MUL_16S_16S(9251, buffer[ 74]))>> 4;
    /* 1 - stage 2 */ pcm_b +=(MUL_16S_16S(8603, buffer[ 74]))>> 6;
    /* 1 - stage 2 */ pcm_a /= 32768; CLIP_INT16(pcm_a); pcm[2<<strideShift] = (OI_INT16)pcm_a;
    /* 1 - stage 2 */ pcm_b /= 32768; CLIP_INT16(pcm_b); pcm[6<<strideShift] = (OI_INT16)pcm_b;
    /* 1 - stage 3 */ pcm_a = 0;
    /* 1 - stage 3 */ pcm_b = 0;
    /* 1 - stage 3 */ pcm_a +=(MUL_16S_16S(-16457, buffer[  7]))>> 6;
    /* 1 - stage 3 */ pcm_b +=(MUL_16S_16S(16913, buffer[  7]))>> 5;
    /* 1 - stage 3 */ pcm_a +=(MUL_16S_16S(19083, buffer[  9]))>> 5;
    /* 1 - stage 3 */ pcm_b +=(MUL_16S_16S(-8443, buffer[  9]))>> 7;
    /* 1 - stage 3 */ pcm_a +=(MUL_16S_16S(-23641, buffer[ 23]))>> 2;
    /* 1 - stage 3 */ pcm_b +=(MUL_16S_16S(3687, buffer[ 23]))<< 1;
    /* 1 - stage 3 */ pcm_a +=(MUL_16S_16S(-29015, buffer[ 25]))>> 4;
    /* 1 - stage 3 */ pcm_b +=(MUL_16S_16S(-301, buffer[ 25]))<< 5;
    /* 1 - stage 3 */ pcm_a +=(MUL_16S_16S(-12889, buffer[ 39]))<< 2;
    /* 1 - stage 3 */ pcm_b +=(MUL_16S_16S(15447, buffer[ 39]))<< 2;
    /* 1 - stage 3 */ pcm_a +=(MUL_16S_16S(6145, buffer[ 41]))<< 3;
    /* 1 - stage 3 */ pcm_b +=(MUL_16S_16S(10255, buffer[ 41]))<< 2;
    /* 1 - stage 3 */ pcm_a +=(MUL_16S_16S(24211, buffer[ 55]))>> 1;
    /* 1 - stage 3 */ pcm_b +=(MUL_16S_16S(-18233, buffer[ 55]))>> 3;
    /* 1 - stage 3 */ pcm_a +=(MUL_16S_16S(23469, buffer[ 57]))>> 2;
    /* 1 - stage 3 */ pcm_b +=(MUL_16S_16S(9405, buffer[ 57]))>> 1;
    /* 1 - stage 3 */ pcm_a +=(MUL_16S_16S(21223, buffer[ 71]))>> 8;
    /* 1 - stage 3 */ pcm_b +=(MUL_16S_16S(1499, buffer[ 71]))>> 1;
    /* 1 - stage 3 */ pcm_a +=(MUL_16S_16S(26913, buffer[ 73]))>> 6;
    /* 1 - stage 3 */ pcm_b +=(MUL_16S_16S(26189, buffer[ 73]))>> 7;
    /* 1 - stage 3 */ pcm_a /= 32768; CLIP_INT16(pcm_a); pcm[3<<strideShift] = (OI_INT16)pcm_a;
    /* 1 - stage 3 */ pcm_b /= 32768; CLIP_INT16(pcm_b); pcm[5<<strideShift] = (OI_INT16)pcm_b;
    /* 1 - stage 4 */ pcm_a = 0;
    /* 1 - stage 4 */ pcm_a +=(MUL_16S_16S(10445, buffer[  8]))>> 4;
    /* 1 - stage 4 */ pcm_a +=(MUL_16S_16S(-5297, buffer[ 24]))<< 1;
    /* 1 - stage 4 */ pcm_a +=(MUL_16S_16S(22299, buffer[ 40]))<< 2;
    /* 1 - stage 4 */ pcm_a +=(MUL_16S_16S(10603, buffer[ 56]));
    /* 1 - stage 4 */ pcm_a +=(MUL_16S_16S(9539, buffer[ 72]))>> 4;
    /* 1 - stage 4 */ pcm_a /= 32768; CLIP_INT16(pcm_a); pcm[4<<strideShift] = (OI_INT16)pcm_a;
}
