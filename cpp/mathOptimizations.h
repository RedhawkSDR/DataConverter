/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of REDHAWK Basic Components DataConverter.
 *
 * REDHAWK Basic Components DataConverter is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * REDHAWK Basic Components DataConverter is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#ifndef MATHOPTIMIZATIONS_H
#define MATHOPTIMIZATIONS_H
#include <stddef.h>
#ifdef __SSE__
#include <xmmintrin.h>
#endif

#ifdef HAVE_IPP
#include <ipp.h>
#endif


#ifdef __SSE2__
#include <emmintrin.h>
#endif
#ifdef __SSE3__
#include <pmmintrin.h>
#endif
#ifdef __SSSE3__
#include <tmmintrin.h>
#endif
#ifdef __SSE4_1__
#include <smmintrin.h>
#endif
#ifdef __SSE4_2__
#include <nmmintrin.h>
#endif
#ifdef __AVX__
#include <immintrin.h>
#endif
#define ALIGNED1(a) ( 0 == ( (ptrdiff_t)(a)&15))
#define ALIGNED2(a,b)  ALIGNED1( (ptrdiff_t)(a)|(ptrdiff_t)(b) )
#define ALIGNED3(a,b,c)  ALIGNED1( (ptrdiff_t)(a)|(ptrdiff_t)(b)|(ptrdiff_t)(c) )

static  __inline
int stack_simd_aligned(void)
{
   __m128 stack_var_simd;
   return ALIGNED1(&stack_var_simd);
}

#ifdef __cplusplus
extern "C" {
#endif


// from char
void char2uchar(unsigned char* dst, const char* src, int n);
void char2short(short* dst, const char* src, int n);
void char2ushort(unsigned short* dst, const  char* src, int n);
void char2float( float *dst, const char* src, int n);
void char2ucharScaled(unsigned char* dst, const char* src, int n);
void char2shortScaled(short* dst, const char* src, int n, short sMin,short dR_dM, short dMin);
void char2ushortScaled(unsigned short* dst, const char* src, int n, short sMin,short dR_dM, short dMin);
void char2floatScaled( float *dst, const char* src, int n, float sMin,float dRange, float sRange, float dMin);

// from uchar
void uchar2char(char* dst, const unsigned char* src, int n);
void uchar2short(short* dst, const unsigned char* src, int n);
void uchar2ushort(unsigned short* dst, const unsigned char* src, int n);
void uchar2float( float *dst, const unsigned char* src, int n);
void uchar2charScaled(char* dst, const unsigned char* src, int n);
void uchar2shortScaled(short* dst, const unsigned char* src, int n, short sMin,short dR_dM, short dMin);
void uchar2ushortScaled(unsigned short* dst, const unsigned char* src, int n, unsigned short sMin,unsigned short dR_dM, unsigned short dMin);
void uchar2floatScaled( float *dst,const  unsigned char* src, int n, float sMin,float dRange, float sRange, float dMin);

// from short
void short2char(char* dst, const short* src, int n);
void short2uchar(unsigned char* dst, const short* src, int n);
void short2ushort(unsigned short* dst, const short* src, int n);
void short2float( float *dst, const short *src, int n);
void short2float2( float *dst, const short *src, int n); // TODO what is this?
void short2charScaled(char* dst, const short* src, int n, float sMin, float dRange, float sRange, float dMin);
void short2ucharScaled(unsigned char* dst, const short* src, int n, float sMin, float dRange, float sRange, float dMin);
void short2ushortScaled( unsigned short *dst, const short *src, int n);
void short2floatScaled( float *dst, const short *src, int n, float sMin,float dRange,float sRange, float dMin);

// from ushort
void ushort2char(char* dst, const unsigned short* src, int n);
void ushort2uchar(unsigned char* dst, const unsigned short* src, int n);
void ushort2short(short* dst, const unsigned short* src, int n);
void ushort2float( float *dst, const unsigned short *src, int n);
void ushort2charScaled(char* dst,const  unsigned short* src, int n, float sMin, float dRange, float sRange, float dMin);
void ushort2ucharScaled(unsigned char* dst,const  unsigned short* src, int n, float sMin, float dRange, float sRange, float dMin);
void ushort2shortScaled(short* dst, const unsigned short* src, int n);
void ushort2floatScaled( float *dst,const  unsigned short *src, int n, float sMin,float dRange,float sRange, float dMin);

// from float
void float2char(char* dst, const float* src, int n);
void float2uchar(unsigned char* dst, const float* src, int n);
void float2short(short* dst, const float* src, int n);
void float2ushort(unsigned short* dst, const float* src, int n);
void float2double(double* dst, const float* src, int n);
void float2charScaled(char* dst, const float* src, int n, float sMin, float dRange, float sRange, float dMin);
void float2ucharScaled(unsigned char* dst,const  float* src, int n, float sMin, float dRange, float sRange, float dMin);
void float2shortScaled(short* dst, const float* src, int n, float sMin, float dRange, float sRange, float dMin);
void float2ushortScaled(unsigned short* dst, const float* src, int n, float sMin, float dRange, float sRange, float dMin);
void float2doubleScaled(double* dst, const float* src, int n, double sMin, double dRange, double sRange, double dMin);
void float2floatScaled(float* dst, const float* src, int n, float sMin, float dRange, float sRange, float dMin);
void interleaveFloat(float* rc, const float *r, const float *c, int n);
void mul_const(float* c, const float *a, const float b, int n);
void copyfloat(float* dst, const float* src, int n);

// from double
void double2doubleScaled(double* dst, const double* src, int n, double sMin, double dRange, double sRange, double dMin);

#ifdef __cplusplus
}
#endif

#endif /* MATHOPTIMIZATIONS_H */

