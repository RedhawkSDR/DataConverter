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

#include "mathOptimizations.h"
void char2ucharScaled(unsigned char* dst, const char* src, int n){
int k = 0;
#ifdef __SSE2__
	__m128i constant = _mm_set1_epi8(0x80);  		
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+15<n;k+=16){
    		__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
  			a1 = _mm_xor_si128(a1,constant);
			_mm_store_si128((__m128i*)(dst+k),a1);			
		}
	}else{
		for( ; k+15<n;k+=16){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
  			a1 = _mm_xor_si128(a1,constant);
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
	}
	if(k<(n)){
		for(;k<n;++k){
			dst[k] = (unsigned char)src[k] -128;
			//dst[k]=temp;
		}						
    }
#endif
}

void char2uchar(unsigned char* dst, const char* src, int n){
int k = 0;
	if(k<(n)){
		for(;k<n;++k){
			dst[k] = (unsigned char)src[k];// -128;
			//dst[k]=temp;
		}						
    }
}

void uchar2char(char* dst, const unsigned char* src, int n){
int k = 0;
	if(k<(n)){
		for(;k<n;++k){
			dst[k]=(char)src[k];
		}						
    }
}

void uchar2charScaled(char* dst, const unsigned char* src, int n){
int k = 0;
#ifdef __SSE2__
	__m128i constant = _mm_set1_epi8(0x80);  		
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+15<n;k+=16){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
  			a1 = _mm_sub_epi8(a1,constant);
			_mm_storeu_si128((__m128i*)(dst+k),a1);
			}
	}else{
		for( ; k+15<n;k+=16){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
  			a1 = _mm_sub_epi8(a1,constant);
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
	}
	if(k<(n)){
		for(;k<n;++k){
			dst[k]=(char)src[k]-128;
		}						
    }
#endif
}
void char2short(short* dst, const char* src, int n){
#ifdef HAVE_IPP_AND_ALWAYS_USE
	ippsConvert_8s16s((const Ipp8s*) src,(Ipp16s*) dst, n);
#else
int k = 0;
#ifdef __SSE2__
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__
			a1 = _mm_cvtepi8_epi16(a1);
#else
			a1 = _mm_unpacklo_epi8(a1,a1);
			a1 = _mm_srai_epi16(a1,0x8);
#endif
			_mm_store_si128((__m128i*)(dst+k),a1);
		}
	
		if(k<(n)){
			for(;k<n;++k){
				dst[k]= (short)(src[k]);
			}							
    		}
	}else{
		for( ; k+7<n;k+=8){
   		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__
			a1 = _mm_cvtepi8_epi16(a1);
#else
			a1 = _mm_unpacklo_epi8(a1,a1);
			a1 = _mm_srai_epi16(a1,0x8);
#endif
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
	
		if(k<(n)){
			for(;k<n;++k){
				dst[k]= (short)(src[k]);
			}							
    		}
	}
//end of ifdef __SSE2___
#endif

//end of ifdef HAVEIPP
#endif

}

void char2ushort(unsigned short* dst, const  char* src, int n){
int k = 0;
#ifdef __SSE2__
	__m128i constant = _mm_set1_epi8(0x80);
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
 			a1 = _mm_xor_si128(a1,constant);
#ifdef __SSE4_1__
			a1 = _mm_cvtepi8_epi16(a1);
#else
			a1 = _mm_unpacklo_epi8(a1,a1);		
			a1 = _mm_srli_epi16(a1,0x8);
#endif
			_mm_store_si128((__m128i*)(dst+k),a1);
		}
	
	if(k<(n)){
		for(;k<n;++k){
			dst[k]= (unsigned short)(src[k]);
		}						
    	}
}else{
		for( ; k+7<n;k+=8){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
 			a1 = _mm_xor_si128(a1,constant);
#ifdef __SSE4_1__
			a1 = _mm_cvtepi8_epi16(a1);
#else
			a1 = _mm_unpacklo_epi8(a1,a1);		
			a1 = _mm_srli_epi16(a1,0x8);
#endif
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
	
	if(k<(n)){
		for(;k<n;++k){
			dst[k]= (unsigned short)(src[k]);
		}						
    	}
}
//end of ifdef __SSE2__
	
#endif
}

void uchar2shortScaled(short* dst, const unsigned char* src, int n, short sMin,short dR_dM, short dMin){
int k = 0;
#ifdef __SSE2__
	//__m128i constant = _mm_set1_epi8(0x80);
	__m128i s_min = _mm_set1_epi16(sMin);
	__m128i range = _mm_set1_epi16(dR_dM);
	__m128i d_min = _mm_set1_epi16(dMin); 		
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__
			a1 = _mm_cvtepi8_epi16(a1);
#else
			a1 = _mm_unpacklo_epi8(a1,a1);
			a1 = _mm_srli_epi16(a1,0x8);
#endif
			a1 = _mm_sub_epi16(a1,s_min);
			a1 = _mm_mullo_epi16(a1,range);
			a1 = _mm_add_epi16(a1,d_min);		
			
			_mm_store_si128((__m128i*)(dst+k),a1);
		}
	
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dR_dM))+dMin;
				dst[k]= (unsigned short)(temp);
			}						
    	}
    }else{
		for( ; k+7<n;k+=8){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__
			a1 = _mm_cvtepi8_epi16(a1);
#else
			a1 = _mm_unpacklo_epi8(a1,a1);
			a1 = _mm_srli_epi16(a1,0x8);
#endif
			a1 = _mm_sub_epi16(a1,s_min);
			a1 = _mm_mullo_epi16(a1,range);
			a1 = _mm_add_epi16(a1,d_min);		
			
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
	
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dR_dM))+dMin;
				dst[k]= (unsigned short)(temp);
			}						
    	}
    
    }
#endif
}

void uchar2ushort(unsigned short* dst, const unsigned char* src, int n){
	int k=0;	
	if(k<(n)){
		for(;k<n;++k){
			dst[k]= (unsigned short)src[k];
		}						
    }
}

void uchar2short(short* dst, const unsigned char* src, int n){
	int k=0;	
	if(k<(n)){
		for(;k<n;++k){
			dst[k]= (short)src[k];
		}						
    }
}


void uchar2ushortScaled(unsigned short* dst, const unsigned char* src, int n, unsigned short sMin,unsigned short dR_dM, unsigned short dMin){
int k = 0;
#ifdef __SSE2__
	//__m128i constant = _mm_set1_epi8(0x80);
	__m128i s_min = _mm_set1_epi16(sMin);
	__m128i range = _mm_set1_epi16(dR_dM);
	__m128i d_min = _mm_set1_epi16(dMin); 		
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__
			a1 = _mm_cvtepi8_epi16(a1);
#else
			a1 = _mm_unpacklo_epi8(a1,a1);
			a1 = _mm_srli_epi16(a1,0x8);
#endif
			a1 = _mm_sub_epi16(a1,s_min);
			a1 = _mm_mullo_epi16(a1,range);
			a1 = _mm_add_epi16(a1,d_min);		
			
			_mm_store_si128((__m128i*)(dst+k),a1);
		}
	
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dR_dM))+dMin;
				dst[k]= (unsigned short)(temp);
			}						
    	}
    }else{
		for( ; k+7<n;k+=8){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__
			a1 = _mm_cvtepi8_epi16(a1);
#else
			a1 = _mm_unpacklo_epi8(a1,a1);
			a1 = _mm_srli_epi16(a1,0x8);
#endif
			a1 = _mm_sub_epi16(a1,s_min);
			a1 = _mm_mullo_epi16(a1,range);
			a1 = _mm_add_epi16(a1,d_min);		
			
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
	
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dR_dM))+dMin;
				dst[k]= (unsigned short)(temp);
			}						
    	}
    
    }
#endif
}


void char2shortScaled(short* dst, const char* src, int n, short sMin,short dR_dM, short dMin){
int k = 0;
#ifdef __SSE2__
	//__m128i constant = _mm_set1_epi8(0x80);
	__m128i s_min = _mm_set1_epi16(sMin);
	__m128i range = _mm_set1_epi16(dR_dM);
	__m128i d_min = _mm_set1_epi16(dMin); 		
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
			//a1 = _mm_xor_si128(a1,constant);
			//a1 = _mm_sub_epi8(a1,s_min);
#ifdef __SSE4_1__
			a1 = _mm_cvtepi8_epi16(a1);
#else
			a1 = _mm_unpacklo_epi8(a1,a1);
			a1 = _mm_srai_epi16(a1,0x8);
#endif
			a1 = _mm_sub_epi16(a1,s_min);
			a1 = _mm_mullo_epi16(a1,range);
			a1 = _mm_add_epi16(a1,d_min);		
			
			_mm_store_si128((__m128i*)(dst+k),a1);
		}
	
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dR_dM))+dMin;
				dst[k]= (short)(temp);
			}						
    		}
	}else{
		for( ; k+7<n;k+=8){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
			//a1 = _mm_xor_si128(a1,constant);
			//a1 = _mm_sub_epi8(a1,s_min);
#ifdef __SSE4_1__
			a1 = _mm_cvtepi8_epi16(a1);
#else
			a1 = _mm_unpacklo_epi8(a1,a1);
			a1 = _mm_srai_epi16(a1,0x8);
#endif
			a1 = _mm_sub_epi16(a1,s_min);
			a1 = _mm_mullo_epi16(a1,range);
			a1 = _mm_add_epi16(a1,d_min);		
			
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
	
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dR_dM))+dMin;
				dst[k]= (short)(temp);
			}						
    		}
	}

#endif
}

void char2ushortScaled(unsigned short* dst, const char* src, int n, short sMin,short dR_dM, short dMin){
int k = 0;
#ifdef __SSE2__
	__m128i constant = _mm_set1_epi8(0x80);
	//__m128i s_min = _mm_set1_epi16(sMin);
	__m128i range = _mm_set1_epi16(dR_dM);
	__m128i d_min = _mm_set1_epi16(dMin); 		
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
 			a1 = _mm_xor_si128(a1,constant);
#ifdef __SSE4_1__
			a1 = _mm_cvtepi8_epi16(a1);
#else
			a1 = _mm_unpacklo_epi8(a1,a1);		
			a1 = _mm_srli_epi16(a1,0x8);
#endif
			a1 = _mm_mullo_epi16(a1,range);
			a1 = _mm_add_epi16(a1,d_min);		
			_mm_store_si128((__m128i*)(dst+k),a1);
		}
	
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dR_dM))+dMin;
				dst[k]= (unsigned short)(temp);
			}						
		}
    
	}else{
		for( ; k+7<n;k+=8){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
 			a1 = _mm_xor_si128(a1,constant);
#ifdef __SSE4_1__
			a1 = _mm_cvtepi8_epi16(a1);
#else
			a1 = _mm_unpacklo_epi8(a1,a1);		
			a1 = _mm_srli_epi16(a1,0x8);
#endif
			a1 = _mm_mullo_epi16(a1,range);
			a1 = _mm_add_epi16(a1,d_min);		
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
	
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dR_dM))+dMin;
				dst[k]= (unsigned short)(temp);
			}						
		}
	}
#endif
}


void char2float( float *dst, const char* src, int n){
#ifdef HAVE_IPP
	(void)ippsConvert_8s32f((const Ipp8s*) src, (Ipp32f*)dst, n);
#else
	int k=0;

#ifdef __SSE4_1__   	
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {    		
		for( ; k+7<n;k+=8){
    		// load 8 16bit words
  			__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
  			__m128i a2 = _mm_load_si128((const __m128i*)( src+k+4));
 			
  		    a1 = _mm_cvtepi8_epi32(a1);
  			a2 = _mm_cvtepi8_epi32(a2);
			
			__m128 c1 = _mm_cvtepi32_ps(a1);
			__m128 c2 = _mm_cvtepi32_ps(a2);
        	
        	_mm_store_ps((float*)(dst+k),c1);
        	_mm_store_ps((float*)(dst+k+4),c2);
		}
		if(k<(n-1)){
			for(;k<n;++k){
				dst[k]=(float)src[k];
			}						
        	}
  	}
#else

#ifdef __SSE2__
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
			__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
			a1 = _mm_unpacklo_epi8(a1,a1);			
			__m128i a2 = _mm_unpackhi_epi16(a1,a1);	
			a1 = _mm_unpacklo_epi16(a1,a1);
			a1 = _mm_srai_epi32(a1,0x18);
			a2 = _mm_srai_epi32(a2,0x18);
			__m128 c1 = _mm_cvtepi32_ps(a1);
			__m128 c2 = _mm_cvtepi32_ps(a2);
			_mm_store_ps((float*)(dst+k),c1);
			_mm_store_ps((float*)(dst+k+4),c2);
		}
		if(k<(n-1)){
			for(;k<n;++k){
				dst[k]=(float)src[k];
			}						
        	}	
	}
#else
	for(;k<n;++k){
		dst[k]=(float)src[k];
	}
#endif
#endif
#endif 	
}
void uchar2float( float *dst, const unsigned char* src, int n){
#ifdef HAVE_IPP
	(void)ippsConvert_8u32f((const Ipp8u*) src, (Ipp32f*)dst, n);
#else
	int k=0;

#ifdef __SSE4_1__   	
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {    		
		for( ; k+7<n;k+=8){
    		// load 8 16bit words
  			__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
  			__m128i a2 = _mm_load_si128((const __m128i*)( src+k+4));
 			
  		    a1 = _mm_cvtepu8_epi32(a1);
  			a2 = _mm_cvtepu8_epi32(a2);
			
			__m128 c1 = _mm_cvtepi32_ps(a1);
			__m128 c2 = _mm_cvtepi32_ps(a2);
        	
        	_mm_store_ps((float*)(dst+k),c1);
        	_mm_store_ps((float*)(dst+k+4),c2);
		}
		if(k<(n-1)){
			for(;k<n;++k){
				dst[k]=(float)src[k];
			}						
        }
  	}
#else

#ifdef __SSE2__
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
			__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
			a1 = _mm_unpacklo_epi8(a1,a1);			
			__m128i a2 = _mm_unpackhi_epi16(a1,a1);	
			a1 = _mm_unpacklo_epi16(a1,a1);
			a1 = _mm_srli_epi32(a1,0x18);
			a2 = _mm_srli_epi32(a2,0x18);
			__m128 c1 = _mm_cvtepi32_ps(a1);
			__m128 c2 = _mm_cvtepi32_ps(a2);
			_mm_store_ps((float*)(dst+k),c1);
			_mm_store_ps((float*)(dst+k+4),c2);
}
		if(k<(n-1)){
			for(;k<n;++k){
				dst[k]=(float)src[k];
			}						
        }	
	}
#else
	for(;k<n;++k){
		dst[k]=(float)src[k];
	}
#endif
#endif
#endif 	
}
void char2floatScaled( float *dst, const char* src, int n, float sMin,float dRange, float sRange, float dMin){
int k=0;
#ifdef __SSE2__	
	__m128 s_min = _mm_set1_ps(sMin);
	__m128 s_range = _mm_set1_ps(sRange);
	__m128 d_range = _mm_set1_ps(dRange);
	__m128 d_min = _mm_set1_ps(dMin);
	
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {    		
		for( ; k+7<n;k+=8){
    		// load 8 16bit words
  		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__ 			
  		__m128i a2 = _mm_load_si128((const __m128i*)( src+k+4));
  		a1 = _mm_cvtepi8_epi32(a1);
  		a2 = _mm_cvtepi8_epi32(a2);
#else			
		a1 = _mm_unpacklo_epi8(a1,a1);			
		__m128i a2 = _mm_unpackhi_epi16(a1,a1);	
		a1 = _mm_unpacklo_epi16(a1,a1);
		a1 = _mm_srai_epi32(a1,0x18);
		a2 = _mm_srai_epi32(a2,0x18);	
#endif	
		__m128 c1 = _mm_cvtepi32_ps(a1);
		__m128 c2 = _mm_cvtepi32_ps(a2);
        	
        	c1 = _mm_sub_ps(c1,s_min);
        	c2 = _mm_sub_ps(c2,s_min);
        	c1 = _mm_mul_ps(c1,d_range);
        	c2 = _mm_mul_ps(c2,d_range);
        	c1 = _mm_div_ps(c1,s_range);
        	c2 = _mm_div_ps(c2,s_range);
        	c1 = _mm_add_ps(c1,d_min);
        	c2 = _mm_add_ps(c2,d_min);
     	
        	_mm_store_ps((float*)(dst+k),c1);
        	_mm_store_ps((float*)(dst+k+4),c2);
		}
		if(k<(n-1)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]=temp;
			}						
        	}
  	}else{
		for( ; k+7<n;k+=8){
    		// load 8 16bit words
  		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__ 			
  		__m128i a2 = _mm_loadu_si128((const __m128i*)( src+k+4));
  		a1 = _mm_cvtepi8_epi32(a1);
  		a2 = _mm_cvtepi8_epi32(a2);
#else			
		a1 = _mm_unpacklo_epi8(a1,a1);			
		__m128i a2 = _mm_unpackhi_epi16(a1,a1);	
		a1 = _mm_unpacklo_epi16(a1,a1);
		a1 = _mm_srai_epi32(a1,0x18);
		a2 = _mm_srai_epi32(a2,0x18);	
#endif	
		__m128 c1 = _mm_cvtepi32_ps(a1);
		__m128 c2 = _mm_cvtepi32_ps(a2);
        	
        	c1 = _mm_sub_ps(c1,s_min);
        	c2 = _mm_sub_ps(c2,s_min);
        	c1 = _mm_mul_ps(c1,d_range);
        	c2 = _mm_mul_ps(c2,d_range);
        	c1 = _mm_div_ps(c1,s_range);
        	c2 = _mm_div_ps(c2,s_range);
        	c1 = _mm_add_ps(c1,d_min);
        	c2 = _mm_add_ps(c2,d_min);
     	
        	_mm_storeu_ps((float*)(dst+k),c1);
        	_mm_storeu_ps((float*)(dst+k+4),c2);
		}
		if(k<(n-1)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]=temp;
			}						
        	}
	
	}
#endif
}

void uchar2floatScaled( float *dst,const  unsigned char* src, int n, float sMin,float dRange, float sRange, float dMin){
int k=0;

#ifdef __SSE2__	
	__m128 s_min = _mm_set1_ps(sMin);
	__m128 s_range = _mm_set1_ps(sRange);
	__m128 d_range = _mm_set1_ps(dRange);
	__m128 d_min = _mm_set1_ps(dMin);
	
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {    		
		for( ; k+7<n;k+=8){
    		// load 8 16bit words
  			__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE_4_1__
			__m128i a2 = _mm_loadu_si128((const __m128i*)( src+k+4));			
			a1 = _mm_cvtepu8_epi32(a1);
  			a2 = _mm_cvtepu8_epi32(a2);
#else
			a1 = _mm_unpacklo_epi8(a1,a1);			
			__m128i a2 = _mm_unpackhi_epi16(a1,a1);	
			a1 = _mm_unpacklo_epi16(a1,a1);
			a1 = _mm_srli_epi32(a1,0x18);
			a2 = _mm_srli_epi32(a2,0x18);
#endif        	
			__m128 c1 = _mm_cvtepi32_ps(a1);
			__m128 c2 = _mm_cvtepi32_ps(a2);

	        	c1 = _mm_sub_ps(c1,s_min);
       	 		c2 = _mm_sub_ps(c2,s_min);
       	 		c1 = _mm_mul_ps(c1,d_range);
       	 		c2 = _mm_mul_ps(c2,d_range);
       	 		c1 = _mm_div_ps(c1,s_range);
       	 		c2 = _mm_div_ps(c2,s_range);
        		c1 = _mm_add_ps(c1,d_min);
        		c2 = _mm_add_ps(c2,d_min);
     	
        		_mm_store_ps((float*)(dst+k),c1);
        		_mm_store_ps((float*)(dst+k+4),c2);
		}

		if(k<n){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]=temp;
			}						
        	}
  	}else{
		for( ; k+7<n;k+=8){
    		// load 8 16bit words
  			__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE_4_1__
			__m128i a2 = _mm_loadu_si128((const __m128i*)( src+k+4));			
			a1 = _mm_cvtepu8_epi32(a1);
  			a2 = _mm_cvtepu8_epi32(a2);
#else
			a1 = _mm_unpacklo_epi8(a1,a1);			
			__m128i a2 = _mm_unpackhi_epi16(a1,a1);	
			a1 = _mm_unpacklo_epi16(a1,a1);
			a1 = _mm_srli_epi32(a1,0x18);
			a2 = _mm_srli_epi32(a2,0x18);
#endif        	
			__m128 c1 = _mm_cvtepi32_ps(a1);
			__m128 c2 = _mm_cvtepi32_ps(a2);

	        	c1 = _mm_sub_ps(c1,s_min);
       	 		c2 = _mm_sub_ps(c2,s_min);
       	 		c1 = _mm_mul_ps(c1,d_range);
       	 		c2 = _mm_mul_ps(c2,d_range);
       	 		c1 = _mm_div_ps(c1,s_range);
       	 		c2 = _mm_div_ps(c2,s_range);
        		c1 = _mm_add_ps(c1,d_min);
        		c2 = _mm_add_ps(c2,d_min);
     	
        		_mm_storeu_ps((float*)(dst+k),c1);
        		_mm_storeu_ps((float*)(dst+k+4),c2);
		}
		if(k<n){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]=temp;
			}						
        	}
	
	}
#endif
}

void short2ucharScaled(unsigned char* dst, const short* src, int n, float sMin, float dRange, float sRange, float dMin){
int k = 0;
#ifdef __SSE2__
	__m128 s_min = _mm_set1_ps(sMin);
	__m128 drange = _mm_set1_ps(dRange);
	__m128 d_min =  _mm_set1_ps(dMin);
	__m128 srange = _mm_set1_ps(sRange);

	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
    		//load values
    		__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__
	   		__m128i a2 = _mm_load_si128((const __m128i*)( src+4+k));
			a1 = _mm_cvtepi16_epi32(a1);
			a2 = _mm_cvtepi16_epi32(a2);
#else
			__m128i a2 = _mm_unpackhi_epi16(a1,a1);
			a1 = _mm_unpacklo_epi16(a1,a1);
			a1 = _mm_srai_epi32(a1,0x10);
			a2 = _mm_srai_epi32(a2,0x10);
#endif
			__m128 c1 =  _mm_cvtepi32_ps(a1);
			__m128 c2 =  _mm_cvtepi32_ps(a2);
			c1 = _mm_sub_ps(c1,s_min);
			
			c2 = _mm_sub_ps(c2,s_min);
			c1 = _mm_mul_ps(c1,drange);
			c2 = _mm_mul_ps(c2,drange);
			c1 = _mm_div_ps(c1,srange);
			c2 = _mm_div_ps(c2,srange);
			c1 = _mm_add_ps(c1,d_min);
			c2 = _mm_add_ps(c2,d_min);
#ifdef __SSE4_1__
			//c1 = _mm_floor_ps(c1);
			//c2 = _mm_float_ps(c2);
#endif
			a1 = _mm_cvtps_epi32(c1);
			a2 = _mm_cvtps_epi32(c2);
			a1 = _mm_packs_epi32(a1,a2);
			a1 = _mm_packus_epi16(a1,a1);
			//a2 = _mm_set1_epi8(0x80);  	
			//a1 = _mm_xor_si128(a1,a2);
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]= (unsigned char)(temp);
			}						
    	}
    
	}else{
			for( ; k+7<n;k+=8){
    		//load values
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__
	   		__m128i a2 = _mm_loadu_si128((const __m128i*)( src+4+k));
			a1 = _mm_cvtepi16_epi32(a1);
			a2 = _mm_cvtepi16_epi32(a2);
#else
			__m128i a2 = _mm_unpackhi_epi16(a1,a1);
			a1 = _mm_unpacklo_epi16(a1,a1);
			a1 = _mm_srai_epi32(a1,0x10);
			a2 = _mm_srai_epi32(a2,0x10);
#endif
			__m128 c1 =  _mm_cvtepi32_ps(a1);
			__m128 c2 =  _mm_cvtepi32_ps(a2);
			c1 = _mm_sub_ps(c1,s_min);			
			c2 = _mm_sub_ps(c2,s_min);

			c1 = _mm_mul_ps(c1,drange);
			c2 = _mm_mul_ps(c2,drange);

			c1 = _mm_div_ps(c1,srange);
			c2 = _mm_div_ps(c2,srange);

			c1 = _mm_add_ps(c1,d_min);
			c2 = _mm_add_ps(c2,d_min);

			a1 = _mm_cvtps_epi32(c1);
			a2 = _mm_cvtps_epi32(c2);
			a1 = _mm_packs_epi32(a1,a2);
			a1 = _mm_packus_epi16(a1,a1);
			
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]= (unsigned char)(temp);
			}						
    	}
    
	}
	
#endif	

} 

void short2uchar(unsigned char* dst, const short* src, int n){
int k = 0;
#ifdef __SSE2__

	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+15<n;k+=16){
    		//load values
    		__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
			__m128i a2 = _mm_load_si128((const __m128i*)(src+k+8));
			a1 = _mm_packus_epi16(a1,a2);
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
		if(k<(n)){
			for(;k<n;++k){
				dst[k]= (unsigned char)(src[k]);
			}						
    		}
	}else{
		for( ; k+15<n;k+=16){
    		//load values
    			__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
	   		__m128i a2 = _mm_loadu_si128((const __m128i*)( src+8+k));
			a1 = _mm_packus_epi16(a1,a1);			
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
		if(k<(n)){
			for(;k<n;++k){
				dst[k]= (unsigned char)(src[k]);
			}						
    		}
	}	
#endif	

} 
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void short2char(char* dst, const short* src, int n){
#ifdef HAVE_IPP_AND_ALWAYS_USE
	ippsConvert_16s8s_Sfs((const Ipp16s*) src,(Ipp8s*) dst, n, ippRndFinancial,0);
#else
int k = 0;
#ifdef __SSE2__
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+15<n;k+=16){
    		__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
    		__m128i a2 = _mm_load_si128((const __m128i*)( src+k));
    		a1 = _mm_packs_epi16(a1,a2);
		_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
	
		if(k<(n)){
			for(;k<n;++k){
				dst[k]= (char)(src[k]);
			}						
    		}
	}else{
		for( ; k+15<n;k+=16){
    			__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
    			__m128i a2 = _mm_loadu_si128((const __m128i*)( src+k));
    			a1 = _mm_packs_epi16(a1,a2);
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
	
		if(k<(n)){
			for(;k<n;++k){
				dst[k]= (char)(src[k]);
			}						
    		}	
	}
#endif
#endif
}

void ushort2char(char* dst, const unsigned short* src, int n){
int k = 0;
	
#ifdef __SSE2__
	__m128i s_min = _mm_set1_epi8(0x80);
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+15<n;k+=16){
    		__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
    		__m128i a2 = _mm_load_si128((const __m128i*)( src+k+8));
    		a1 = _mm_packus_epi16(a1,a2);
    		a1 = _mm_sub_epi8(a1,s_min); 
		_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
		
		if(k<(n)){
			for(;k<n;++k){
				dst[k]= (char)(src[k]);
			}
		}
	}else{	
	for( ; k+15<n;k+=16){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
    		__m128i a2 = _mm_loadu_si128((const __m128i*)( src+k+8));
    		a1 = _mm_packus_epi16(a1,a2);
    		a1 = _mm_sub_epi8(a1,s_min); 
		_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
		
		if(k<(n)){
			for(;k<n;++k){
				dst[k]= (char)(src[k]);
			}
		}
	
	}					
    
#endif
}

void ushort2charScaled(char* dst,const  unsigned short* src, int n, float sMin, float dRange, float sRange, float dMin){
int k = 0;
#ifdef __SSE2__
	__m128 s_min = _mm_set1_ps(sMin);
	__m128 drange = _mm_set1_ps(dRange);
	__m128 d_min =  _mm_set1_ps(dMin);
	__m128 srange = _mm_set1_ps(sRange);

	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
			__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__
	   		__m128i a2 = _mm_load_si128((const __m128i*)( src+4+k));
			a1 = _mm_cvtepu16_epi32(a1);
			a2 = _mm_cvtepu16_epi32(a2);
#else
			__m128i a2 = _mm_unpackhi_epi16(a1,a1);
			a1 = _mm_unpacklo_epi16(a1,a1);
			a1 = _mm_srli_epi32(a1,0x10);
			a2 = _mm_srli_epi32(a2,0x10);
#endif
			__m128 c1 =  _mm_cvtepi32_ps(a1);
			__m128 c2 =  _mm_cvtepi32_ps(a2);
			c1 = _mm_sub_ps(c1,s_min);
			
			c2 = _mm_sub_ps(c2,s_min);
			c1 = _mm_mul_ps(c1,drange);
			c2 = _mm_mul_ps(c2,drange);
			c1 = _mm_div_ps(c1,srange);
			c2 = _mm_div_ps(c2,srange);
			c1 = _mm_add_ps(c1,d_min);
			c2 = _mm_add_ps(c2,d_min);

			a1 = _mm_cvtps_epi32(c1);
			a2 = _mm_cvtps_epi32(c2);
			a1 = _mm_packs_epi32(a1,a2);
			a1 = _mm_packs_epi16(a1,a1);
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]= (char)(temp);
			}						
    		}
	}else{
		for( ; k+7<n;k+=8){
			__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__
	   		__m128i a2 = _mm_loadu_si128((const __m128i*)( src+4+k));
			a1 = _mm_cvtepu16_epi32(a1);
			a2 = _mm_cvtepu16_epi32(a2);
#else
			__m128i a2 = _mm_unpackhi_epi16(a1,a1);
			a1 = _mm_unpacklo_epi16(a1,a1);
			a1 = _mm_srli_epi32(a1,0x10);
			a2 = _mm_srli_epi32(a2,0x10);
#endif
			__m128 c1 =  _mm_cvtepi32_ps(a1);
			__m128 c2 =  _mm_cvtepi32_ps(a2);
			c1 = _mm_sub_ps(c1,s_min);
			
			c2 = _mm_sub_ps(c2,s_min);
			c1 = _mm_mul_ps(c1,drange);
			c2 = _mm_mul_ps(c2,drange);
			c1 = _mm_div_ps(c1,srange);
			c2 = _mm_div_ps(c2,srange);
			c1 = _mm_add_ps(c1,d_min);
			c2 = _mm_add_ps(c2,d_min);

			a1 = _mm_cvtps_epi32(c1);
			a2 = _mm_cvtps_epi32(c2);
			a1 = _mm_packs_epi32(a1,a2);
			a1 = _mm_packs_epi16(a1,a1);
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]= (char)(temp);
			}						
    		}
	


	}
#endif
}

void ushort2ucharScaled(unsigned char* dst,const  unsigned short* src, int n, float sMin, float dRange, float sRange, float dMin){
int k = 0;
#ifdef __SSE2__
	__m128 s_min = _mm_set1_ps(sMin);
	__m128 drange = _mm_set1_ps(dRange);
	__m128 d_min =  _mm_set1_ps(dMin);
	__m128 srange = _mm_set1_ps(sRange);

	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
			__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__
	   		__m128i a2 = _mm_load_si128((const __m128i*)( src+4+k));
			a1 = _mm_cvtepu16_epi32(a1);
			a2 = _mm_cvtepu16_epi32(a2);
#else
			__m128i a2 = _mm_unpackhi_epi16(a1,a1);
			a1 = _mm_unpacklo_epi16(a1,a1);
			a1 = _mm_srli_epi32(a1,0x10);
			a2 = _mm_srli_epi32(a2,0x10);
#endif
			__m128 c1 =  _mm_cvtepi32_ps(a1);
			__m128 c2 =  _mm_cvtepi32_ps(a2);
			c1 = _mm_sub_ps(c1,s_min);
			
			c2 = _mm_sub_ps(c2,s_min);
			c1 = _mm_mul_ps(c1,drange);
			c2 = _mm_mul_ps(c2,drange);
			c1 = _mm_div_ps(c1,srange);
			c2 = _mm_div_ps(c2,srange);
			c1 = _mm_add_ps(c1,d_min);
			c2 = _mm_add_ps(c2,d_min);

			a1 = _mm_cvtps_epi32(c1);
			a2 = _mm_cvtps_epi32(c2);
			a1 = _mm_packs_epi32(a1,a2);
			a1 = _mm_packus_epi16(a1,a1);
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]= (unsigned char)(temp);
			}						
    		}
	}else{
		for( ; k+7<n;k+=8){
			__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__
	   		__m128i a2 = _mm_loadu_si128((const __m128i*)( src+4+k));
			a1 = _mm_cvtepu16_epi32(a1);
			a2 = _mm_cvtepu16_epi32(a2);
#else
			__m128i a2 = _mm_unpackhi_epi16(a1,a1);
			a1 = _mm_unpacklo_epi16(a1,a1);
			a1 = _mm_srli_epi32(a1,0x10);
			a2 = _mm_srli_epi32(a2,0x10);
#endif
			__m128 c1 =  _mm_cvtepi32_ps(a1);
			__m128 c2 =  _mm_cvtepi32_ps(a2);

			c1 = _mm_sub_ps(c1,s_min);			
			c2 = _mm_sub_ps(c2,s_min);
			c1 = _mm_mul_ps(c1,drange);
			c2 = _mm_mul_ps(c2,drange);
			c1 = _mm_div_ps(c1,srange);
			c2 = _mm_div_ps(c2,srange);
			c1 = _mm_add_ps(c1,d_min);
			c2 = _mm_add_ps(c2,d_min);
			a1 = _mm_cvtps_epi32(c1);
			a2 = _mm_cvtps_epi32(c2);
			a1 = _mm_packs_epi32(a1,a2);
			a1 = _mm_packus_epi16(a1,a1);
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]= (unsigned char)(temp);
			}						
    		}
	


	}
#endif
}

void short2charScaled(char* dst, const short* src, int n, float sMin, float dRange, float sRange, float dMin){
int k = 0;
#ifdef __SSE2__
	__m128 s_min = _mm_set1_ps(sMin);
	__m128 drange = _mm_set1_ps(dRange);
	__m128 d_min =  _mm_set1_ps(dMin);
	__m128 srange = _mm_set1_ps(sRange);

	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
	    		__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__
	   		__m128i a2 = _mm_load_si128((const __m128i*)( src+4+k));
			a1 = _mm_cvtepi16_epi32(a1);
			a2 = _mm_cvtepi16_epi32(a2);
#else
			__m128i a2 = _mm_unpackhi_epi16(a1,a1);
			a1 = _mm_unpacklo_epi16(a1,a1);
			a1 = _mm_srai_epi32(a1,0x10);
			a2 = _mm_srai_epi32(a2,0x10);
#endif
			__m128 c1 =  _mm_cvtepi32_ps(a1);
			__m128 c2 =  _mm_cvtepi32_ps(a2);
			c1 = _mm_sub_ps(c1,s_min);
			
			c2 = _mm_sub_ps(c2,s_min);
			c1 = _mm_mul_ps(c1,drange);
			c2 = _mm_mul_ps(c2,drange);
			c1 = _mm_div_ps(c1,srange);
			c2 = _mm_div_ps(c2,srange);
			c1 = _mm_add_ps(c1,d_min);
			c2 = _mm_add_ps(c2,d_min);

			a1 = _mm_cvtps_epi32(c1);
			a2 = _mm_cvtps_epi32(c2);
			a1 = _mm_packs_epi32(a1,a2);
			a1 = _mm_packs_epi16(a1,a1);
			
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]= (char)(temp);
			}						
    		}
	}else{
		for( ; k+7<n;k+=8){
	    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__
	   		__m128i a2 = _mm_loadu_si128((const __m128i*)( src+4+k));
			a1 = _mm_cvtepi16_epi32(a1);
			a2 = _mm_cvtepi16_epi32(a2);
#else
			__m128i a2 = _mm_unpackhi_epi16(a1,a1);
			a1 = _mm_unpacklo_epi16(a1,a1);
			a1 = _mm_srai_epi32(a1,0x10);
			a2 = _mm_srai_epi32(a2,0x10);
#endif
			__m128 c1 =  _mm_cvtepi32_ps(a1);
			__m128 c2 =  _mm_cvtepi32_ps(a2);
			c1 = _mm_sub_ps(c1,s_min);
			
			c2 = _mm_sub_ps(c2,s_min);
			c1 = _mm_mul_ps(c1,drange);
			c2 = _mm_mul_ps(c2,drange);
			c1 = _mm_div_ps(c1,srange);
			c2 = _mm_div_ps(c2,srange);
			c1 = _mm_add_ps(c1,d_min);
			c2 = _mm_add_ps(c2,d_min);

			a1 = _mm_cvtps_epi32(c1);
			a2 = _mm_cvtps_epi32(c2);
			a1 = _mm_packs_epi32(a1,a2);
			a1 = _mm_packs_epi16(a1,a1);
			
			_mm_storeu_si128((__m128i*)(dst+k),a1);
		}
		if(k<(n)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]= (char)(temp);
			}						
    		}
	
	}
#endif	
//#endif
} 

void short2ushortScaled(unsigned short* dst, const short* src, int n){
int k = 0;
#ifdef __SSE2__
	__m128i constant = _mm_set1_epi16(0x8000);  		
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+15<n;k+=16){
    		__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
  			__m128i a2 = _mm_load_si128((const __m128i*)( src+k+8));
			a1 = _mm_xor_si128(a1,constant);
			a2 = _mm_xor_si128(a2,constant);
			_mm_store_si128((__m128i*)(dst+k),a1);
			_mm_store_si128((__m128i*)(dst+k+8),a2);	
		}
		if(k<(n)){
			for(;k<n;++k){
				float temp = src[k] -32768;
				dst[k]=(unsigned short)(temp);
			}							
    	}
   	}else{
   		for( ; k+15<n;k+=16){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
  			__m128i a2 = _mm_loadu_si128((const __m128i*)( src+k+8));
			a1 = _mm_add_epi16(a1,constant);
			a2 = _mm_add_epi16(a2,constant);
			_mm_storeu_si128((__m128i*)(dst+k),a1);
			_mm_storeu_si128((__m128i*)(dst+k+8),a2);
			
		}
		if(k<(n)){
			for(;k<n;++k){
				float temp = src[k] - 32768;
				dst[k]=(unsigned short)(temp);
			}							
    	}
	}
#endif
}

void short2ushort(unsigned short* dst, const short* src, int n){
int k = 0;
	if(k<(n)){
		for(;k<n;++k){
			dst[k]=(unsigned short)(src[k]);
		}							
    }
}


void ushort2short(short* dst, const unsigned short* src, int n){
int k = 0;
	if(k<(n)){
		for(;k<n;++k){
			dst[k]=(short)(src[k]);
		}							
    }
}

void ushort2shortScaled(short* dst, const unsigned short* src, int n){
int k = 0;
#ifdef __SSE2__
	__m128i constant = _mm_set1_epi16(0x8000);  		
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+15<n;k+=16){
    		__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
  			__m128i a2 = _mm_load_si128((const __m128i*)( src+k+8));
			a1 = _mm_sub_epi16(a1,constant);
			a2 = _mm_sub_epi16(a2,constant);
			_mm_store_si128((__m128i*)(dst+k),a1);
			_mm_store_si128((__m128i*)(dst+k+8),a2);
			
		}
	
		if(k<(n)){
			for(;k<n;++k){
				float temp = src[k] - 32768;
				dst[k] = (short)temp;
			}							
	    }
   	}else{
		for( ; k+15<n;k+=16){
    			__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
  			__m128i a2 = _mm_loadu_si128((const __m128i*)( src+k+8));
			a1 = _mm_sub_epi16(a1,constant);
			a2 = _mm_sub_epi16(a2,constant);
			_mm_storeu_si128((__m128i*)(dst+k),a1);
			_mm_storeu_si128((__m128i*)(dst+k+8),a2);
			
		}
		if(k<(n)){
			for(;k<n;++k){
				float temp = src[k] - 32768;
				dst[k] = (short) temp;
			}							
    	}
	}
#endif
}

void short2float( float *dst, const short *src, int n){
       //Extract integer word from packed integer array element selected by index
       #ifdef HAVE_IPP_AND_ALWAYS_USE
       	
       	ippsConvert_16s32f((const Ipp16s*) src, (Ipp32f*)dst, n);
       #else
       	int k=0;
       #ifdef __SSE4_1__
       	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {

       		for( ; k+15<n;k+=16){
           		__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
         			__m128i a2 = _mm_load_si128((const __m128i*)( src+k+4));
         			__m128i a3 = _mm_load_si128((const __m128i*)( src+k+8));
         			__m128i a4 = _mm_load_si128((const __m128i*)( src+k+12));
         			a1 = _mm_cvtepi16_epi32(a1);
         			a2 = _mm_cvtepi16_epi32(a2);
       			a3 = _mm_cvtepi16_epi32(a3);
         			a4 = _mm_cvtepi16_epi32(a4);
       			__m128 c1 = _mm_cvtepi32_ps(a1);
       			__m128 c2 = _mm_cvtepi32_ps(a2);
       			__m128 c3 = _mm_cvtepi32_ps(a3);
       			__m128 c4 = _mm_cvtepi32_ps(a4);
               	_mm_store_ps((float*)(dst+k),c1);
               	_mm_store_ps((float*)(dst+k+4),c2);
               	_mm_store_ps((float*)(dst+k+8),c3);
               	_mm_store_ps((float*)(dst+k+12),c4);
       		}
       		if(k<(n-1)){
       			for(;k<n;++k){
       				dst[k]=(float)src[k];
       			}
               }
       	}else{
         		for( ; k+15<n;k+=16){
              		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
          			__m128i a2 = _mm_loadu_si128((const __m128i*)( src+k+4));
          			__m128i a3 = _mm_loadu_si128((const __m128i*)( src+k+8));
          			__m128i a4 = _mm_loadu_si128((const __m128i*)( src+k+12));
          					a1 = _mm_cvtepi16_epi32(a1);
                 			a2 = _mm_cvtepi16_epi32(a2);
               			a3 = _mm_cvtepi16_epi32(a3);
                 			a4 = _mm_cvtepi16_epi32(a4);
               			__m128 c1 = _mm_cvtepi32_ps(a1);
               			__m128 c2 = _mm_cvtepi32_ps(a2);
               			__m128 c3 = _mm_cvtepi32_ps(a3);
               			__m128 c4 = _mm_cvtepi32_ps(a4);
                       	_mm_storeu_ps((float*)(dst+k),c1);
                       	_mm_storeu_ps((float*)(dst+k+4),c2);
                       	_mm_storeu_ps((float*)(dst+k+8),c3);
                       	_mm_storeu_ps((float*)(dst+k+12),c4);
               	}
               	if(k<(n-1)){
               		for(;k<n;++k){
               			dst[k]=(float)src[k];
               		}
                   }
       	}

       #else
       #ifdef __SSE2__
       	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
       
       		for( ; k+15<n;k+=16){

       	   		__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
       	   		__m128i a3 = _mm_load_si128((const __m128i*)( src+k+8));
       	   		__m128i a2 = a1;
       			__m128i a4 = a3;

       	   		a1 = _mm_unpacklo_epi16(a1,a1);
         			a2 = _mm_unpackhi_epi16(a2,a2);
       			a3 = _mm_unpacklo_epi16(a3,a3);
       			a4 = _mm_unpackhi_epi16(a4,a4);

         			a1 = _mm_srai_epi32(a1,0x10);
         			a2 = _mm_srai_epi32(a2,0x10);
         			a3 = _mm_srai_epi32(a3,0x10);
         			a4 = _mm_srai_epi32(a4,0x10);

         			__m128 c1 = _mm_cvtepi32_ps(a1);
         			__m128 c2 = _mm_cvtepi32_ps(a2);
       			__m128 c3 = _mm_cvtepi32_ps(a3);
       			__m128 c4 = _mm_cvtepi32_ps(a4);

       			_mm_store_ps((float*)(dst+k),c1);
             		_mm_store_ps((float*)(dst+k+4),c2);
       			_mm_store_ps((float*)(dst+k+8),c3);
             		_mm_store_ps((float*)(dst+k+12),c4);
       		}
       		if(k<(n-1)){
       			for(;k<n;++k){
       				dst[k]=(float)src[k];
       			}
               }
       	}
       	else{
      
       	for( ; k+15<n;k+=16){

       	   		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
       	   		__m128i a3 = _mm_loadu_si128((const __m128i*)( src+k+8));
       	   		__m128i a2 = a1;
       			__m128i a4 = a3;

       	   		a1 = _mm_unpacklo_epi16(a1,a1);
         			a2 = _mm_unpackhi_epi16(a2,a2);
       			a3 = _mm_unpacklo_epi16(a3,a3);
       			a4 = _mm_unpackhi_epi16(a4,a4);

         			a1 = _mm_srai_epi32(a1,0x10);
         			a2 = _mm_srai_epi32(a2,0x10);
         			a3 = _mm_srai_epi32(a3,0x10);
         			a4 = _mm_srai_epi32(a4,0x10);

         			__m128 c1 = _mm_cvtepi32_ps(a1);
         			__m128 c2 = _mm_cvtepi32_ps(a2);
       			__m128 c3 = _mm_cvtepi32_ps(a3);
       			__m128 c4 = _mm_cvtepi32_ps(a4);

       			_mm_storeu_ps((float*)(dst+k),c1);
             		_mm_storeu_ps((float*)(dst+k+4),c2);
       			_mm_storeu_ps((float*)(dst+k+8),c3);
             		_mm_storeu_ps((float*)(dst+k+12),c4);
       		}
       		if(k<(n-1)){
       			for(;k<n;++k){
       				dst[k]=(float)src[k];
       			}
               }
       	}

       #else

       	for(;k<n;++k){
       		dst[k]=(float)src[k];
       	}
       #endif
       #endif
       #endif
       }


void ushort2float( float *dst, const unsigned short *src, int n){
//Extract integer word from packed integer array element selected by index
#ifdef HAVE_IPP_AND_ALWAYS_USE
	ippsConvert_16s32f((const Ipp16s*) src, (Ipp32f*)dst, n);
#else
	int k=0;
#ifdef __SSE4_1__ 
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
	
		for( ; k+15<n;k+=16){
    		__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
  			__m128i a2 = _mm_load_si128((const __m128i*)( src+k+4));
  			__m128i a3 = _mm_load_si128((const __m128i*)( src+k+8));
  			__m128i a4 = _mm_load_si128((const __m128i*)( src+k+12));
  			a1 = _mm_cvtepu16_epi32(a1);			
  			a2 = _mm_cvtepu16_epi32(a2);
			a3 = _mm_cvtepu16_epi32(a3);
  			a4 = _mm_cvtepu16_epi32(a4);
			__m128 c1 = _mm_cvtepi32_ps(a1);			
			__m128 c2 = _mm_cvtepi32_ps(a2);
			__m128 c3 = _mm_cvtepi32_ps(a3);
			__m128 c4 = _mm_cvtepi32_ps(a4);			
        	_mm_store_ps((float*)(dst+k),c1);
        	_mm_store_ps((float*)(dst+k+4),c2);
        	_mm_store_ps((float*)(dst+k+8),c3);
        	_mm_store_ps((float*)(dst+k+12),c4);     	
		}
		if(k<(n-1)){
			for(;k<n;++k){
				dst[k]=(float)src[k];
			}						
        }	
	}
#else
#ifdef __SSE2__
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+15<n;k+=16){
			
	   		__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
	   		__m128i a3 = _mm_load_si128((const __m128i*)( src+k+8));
	   		__m128i a2 = a1;
			__m128i a4 = a3;

	   		a1 = _mm_unpacklo_epi16(a1,a1);
  			a2 = _mm_unpackhi_epi16(a2,a2);
			a3 = _mm_unpacklo_epi16(a3,a3);
			a4 = _mm_unpackhi_epi16(a4,a4);

  			a1 = _mm_srli_epi32(a1,0x10);
  			a2 = _mm_srli_epi32(a2,0x10); 
  			a3 = _mm_srli_epi32(a3,0x10);
  			a4 = _mm_srli_epi32(a4,0x10);

  			__m128 c1 = _mm_cvtepi32_ps(a1); 			
  			__m128 c2 = _mm_cvtepi32_ps(a2);
			__m128 c3 = _mm_cvtepi32_ps(a3);
			__m128 c4 = _mm_cvtepi32_ps(a4);
			
			_mm_store_ps((float*)(dst+k),c1);
      		_mm_store_ps((float*)(dst+k+4),c2);
			_mm_store_ps((float*)(dst+k+8),c3);
      		_mm_store_ps((float*)(dst+k+12),c4);			
		}
		if(k<(n-1)){
			for(;k<n;++k){
				dst[k]=(float)src[k];
			}						
        }	
	}else{
		for( ; k+15<n;k+=16){
			
	   		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
	   		__m128i a3 = _mm_loadu_si128((const __m128i*)( src+k+8));
	   		__m128i a2 = a1;
			__m128i a4 = a3;

	   		a1 = _mm_unpacklo_epi16(a1,a1);
  			a2 = _mm_unpackhi_epi16(a2,a2);
			a3 = _mm_unpacklo_epi16(a3,a3);
			a4 = _mm_unpackhi_epi16(a4,a4);

  			a1 = _mm_srli_epi32(a1,0x10);
  			a2 = _mm_srli_epi32(a2,0x10); 
  			a3 = _mm_srli_epi32(a3,0x10);
  			a4 = _mm_srli_epi32(a4,0x10);

  			__m128 c1 = _mm_cvtepi32_ps(a1); 			
  			__m128 c2 = _mm_cvtepi32_ps(a2);
			__m128 c3 = _mm_cvtepi32_ps(a3);
			__m128 c4 = _mm_cvtepi32_ps(a4);
			
			_mm_storeu_ps((float*)(dst+k),c1);
      			_mm_storeu_ps((float*)(dst+k+4),c2);
			_mm_storeu_ps((float*)(dst+k+8),c3);
      			_mm_storeu_ps((float*)(dst+k+12),c4);			
		}
		if(k<(n-1)){
			for(;k<n;++k){
				dst[k]=(float)src[k];
			}						
        }	
	}
#else
	for(;k<n;++k){
		dst[k]=(float)src[k];
	}
#endif
#endif
#endif
} 
/*
void short2ushortScaled( float *dst, const short *src, int n, float sMin,float dRange,float sRange, float dMin){
	int k=0;
#ifdef __SSE2__
	__m128 s_min = _mm_set1_ps(sMin);
	__m128 s_range = _mm_set1_ps(sRange);
	__m128 d_range = _mm_set1_ps(dRange);
	__m128 d_min = _mm_set1_ps(dMin);
 
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
    			__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__  			
  			__m128i a2 = _mm_load_si128((const __m128i*)( src+k+4));
			a1 = _mm_cvtepi16_epi32(a1);			
  			a2 = _mm_cvtepi16_epi32(a2);
#else
			__m128i a2 = a1;
			a1 = _mm_unpacklo_epi16(a1,a1);
  			a2 = _mm_unpackhi_epi16(a2,a2);
  			a1 = _mm_srai_epi32(a1,0x10);
  			a2 = _mm_srai_epi32(a2,0x10); 
  			
#endif			
			__m128 c1 = _mm_cvtepi32_ps(a1);			
			__m128 c2 = _mm_cvtepi32_ps(a2);
			
			c1 = _mm_sub_ps(c1,s_min);
        		c2 = _mm_sub_ps(c2,s_min);
        		c1 = _mm_mul_ps(c1,d_range);
        		c2 = _mm_mul_ps(c2,d_range);
        		c1 = _mm_div_ps(c1,s_range);
        		c2 = _mm_div_ps(c2,s_range);
        		c1 = _mm_add_ps(c1,d_min);
        		c2 = _mm_add_ps(c2,d_min);
        		_mm_store_ps((float*)(dst+k),c1);
        		_mm_store_ps((float*)(dst+k+4),c2);
		}
		if(k<(n-1)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]=(float)temp;
			}						
        	}	
	}else{
		for( ; k+7<n;k+=8){
    			__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__  			
  			__m128i a2 = _mm_loadu_si128((const __m128i*)( src+k+4));
			a1 = _mm_cvtepi16_epi32(a1);			
  			a2 = _mm_cvtepi16_epi32(a2);
#else
			__m128i a2 = a1;
			a1 = _mm_unpacklo_epi16(a1,a1);
  			a2 = _mm_unpackhi_epi16(a2,a2);
  			a1 = _mm_srai_epi32(a1,0x10);
  			a2 = _mm_srai_epi32(a2,0x10); 
  			
#endif			
			__m128 c1 = _mm_cvtepi32_ps(a1);			
			__m128 c2 = _mm_cvtepi32_ps(a2);
			
			c1 = _mm_sub_ps(c1,s_min);
        		c2 = _mm_sub_ps(c2,s_min);
        		c1 = _mm_mul_ps(c1,d_range);
        		c2 = _mm_mul_ps(c2,d_range);
        		c1 = _mm_div_ps(c1,s_range);
        		c2 = _mm_div_ps(c2,s_range);
        		c1 = _mm_add_ps(c1,d_min);
        		c2 = _mm_add_ps(c2,d_min);
        		_mm_storeu_ps((float*)(dst+k),c1);
        		_mm_storeu_ps((float*)(dst+k+4),c2);
		}
		if(k<(n-1)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]=(float)temp;
			}						
        	}	
		
	}
#endif
} 
*/
void short2floatScaled( float *dst, const short *src, int n, float sMin,float dRange,float sRange, float dMin){
	int k=0;
#ifdef __SSE2__
	__m128 s_min = _mm_set1_ps(sMin);
	__m128 s_range = _mm_set1_ps(sRange);
	__m128 d_range = _mm_set1_ps(dRange);
	__m128 d_min = _mm_set1_ps(dMin);
 
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
    			__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__  			
  			__m128i a2 = _mm_load_si128((const __m128i*)( src+k+4));
			a1 = _mm_cvtepi16_epi32(a1);			
  			a2 = _mm_cvtepi16_epi32(a2);
#else
			__m128i a2 = a1;
			a1 = _mm_unpacklo_epi16(a1,a1);
  			a2 = _mm_unpackhi_epi16(a2,a2);
  			a1 = _mm_srai_epi32(a1,0x10);
  			a2 = _mm_srai_epi32(a2,0x10); 
  			
#endif			
			__m128 c1 = _mm_cvtepi32_ps(a1);			
			__m128 c2 = _mm_cvtepi32_ps(a2);
			
			c1 = _mm_sub_ps(c1,s_min);
        		c2 = _mm_sub_ps(c2,s_min);
        		c1 = _mm_mul_ps(c1,d_range);
        		c2 = _mm_mul_ps(c2,d_range);
        		c1 = _mm_div_ps(c1,s_range);
        		c2 = _mm_div_ps(c2,s_range);
        		c1 = _mm_add_ps(c1,d_min);
        		c2 = _mm_add_ps(c2,d_min);
        		_mm_store_ps((float*)(dst+k),c1);
        		_mm_store_ps((float*)(dst+k+4),c2);
		}
		if(k<(n-1)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]=(float)temp;
			}						
        	}	
	}else{
		for( ; k+7<n;k+=8){
    			__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__  			
  			__m128i a2 = _mm_loadu_si128((const __m128i*)( src+k+4));
			a1 = _mm_cvtepi16_epi32(a1);			
  			a2 = _mm_cvtepi16_epi32(a2);
#else
			__m128i a2 = a1;
			a1 = _mm_unpacklo_epi16(a1,a1);
  			a2 = _mm_unpackhi_epi16(a2,a2);
  			a1 = _mm_srai_epi32(a1,0x10);
  			a2 = _mm_srai_epi32(a2,0x10); 
  			
#endif			
			__m128 c1 = _mm_cvtepi32_ps(a1);			
			__m128 c2 = _mm_cvtepi32_ps(a2);
			
			c1 = _mm_sub_ps(c1,s_min);
        		c2 = _mm_sub_ps(c2,s_min);
        		c1 = _mm_mul_ps(c1,d_range);
        		c2 = _mm_mul_ps(c2,d_range);
        		c1 = _mm_div_ps(c1,s_range);
        		c2 = _mm_div_ps(c2,s_range);
        		c1 = _mm_add_ps(c1,d_min);
        		c2 = _mm_add_ps(c2,d_min);
        		_mm_storeu_ps((float*)(dst+k),c1);
        		_mm_storeu_ps((float*)(dst+k+4),c2);
		}
		if(k<(n-1)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]=(float)temp;
			}						
        	}	
		
	}
#endif
} 
void ushort2floatScaled( float *dst,const  unsigned short *src, int n, float sMin,float dRange,float sRange, float dMin){
//Extract integer word from packed integer array element selected by index
	int k=0;
#ifdef __SSE2__
	__m128 s_min = _mm_set1_ps(sMin);
	__m128 s_range = _mm_set1_ps(sRange);
	__m128 d_range = _mm_set1_ps(dRange);
	__m128 d_min = _mm_set1_ps(dMin);
	
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
	    		__m128i a1 = _mm_load_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__ 
			__m128i a2 = _mm_load_si128((const __m128i*)( src+k+4));
  			a1 = _mm_cvtepu16_epi32(a1);			
  			a2 = _mm_cvtepu16_epi32(a2);
#else
			__m128i a2 = a1;
	   		a1 = _mm_unpacklo_epi16(a1,a1);
  			a2 = _mm_unpackhi_epi16(a2,a2);
			
  			a1 = _mm_srli_epi32(a1,0x10);
  			a2 = _mm_srli_epi32(a2,0x10); 
#endif
			__m128 c1 = _mm_cvtepi32_ps(a1);			
			__m128 c2 = _mm_cvtepi32_ps(a2);
			c1 = _mm_sub_ps(c1,s_min);
      	  		c2 = _mm_sub_ps(c2,s_min);
        		c1 = _mm_mul_ps(c1,d_range);
	        	c2 = _mm_mul_ps(c2,d_range);
        		c1 = _mm_div_ps(c1,s_range);
       	 		c2 = _mm_div_ps(c2,s_range);
       	 		c1 = _mm_add_ps(c1,d_min);
       		 	c2 = _mm_add_ps(c2,d_min);
        		_mm_store_ps((float*)(dst+k),c1);
        		_mm_store_ps((float*)(dst+k+4),c2);
			}
			if(k<(n-1)){
				for(;k<n;++k){
					float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
					dst[k]=temp;
				}						
        	}	
	}else{
		for( ; k+7<n;k+=8){
	    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
#ifdef __SSE4_1__ 
			__m128i a2 = _mm_loadu_si128((const __m128i*)( src+k+4));
  			a1 = _mm_cvtepu16_epi32(a1);			
  			a2 = _mm_cvtepu16_epi32(a2);
#else
			__m128i a2 = a1;
	   		a1 = _mm_unpacklo_epi16(a1,a1);
  			a2 = _mm_unpackhi_epi16(a2,a2);
			
  			a1 = _mm_srli_epi32(a1,0x10);
  			a2 = _mm_srli_epi32(a2,0x10); 
#endif
			__m128 c1 = _mm_cvtepi32_ps(a1);			
			__m128 c2 = _mm_cvtepi32_ps(a2);
			c1 = _mm_sub_ps(c1,s_min);
      	  		c2 = _mm_sub_ps(c2,s_min);
        		c1 = _mm_mul_ps(c1,d_range);
	        	c2 = _mm_mul_ps(c2,d_range);
        		c1 = _mm_div_ps(c1,s_range);
       	 		c2 = _mm_div_ps(c2,s_range);
       	 		c1 = _mm_add_ps(c1,d_min);
       		 	c2 = _mm_add_ps(c2,d_min);
        		_mm_storeu_ps((float*)(dst+k),c1);
        		_mm_storeu_ps((float*)(dst+k+4),c2);
		}
		if(k<(n-1)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]=(float)temp;
			}						
        }	
	} 
#endif
}

void short2float2( float *dst, const short *src, int n){
	int k = 0;

	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for( ; k+7<n;k+=8){
    		__m128i a1 = _mm_loadu_si128((const __m128i*)( src+k));
  			__m128i a2 = _mm_loadu_si128((const __m128i*)( src+k+4));
  			__m128i xlo = _mm_unpacklo_epi16(a1,_mm_set1_epi16(0));
  			__m128i xhi = _mm_unpackhi_epi16(a1,_mm_set1_epi16(0));
  			__m128 c1 = _mm_cvtepi32_ps(xlo);
  			__m128 c2 = _mm_cvtepi32_ps(xhi);
  			_mm_store_ps((float*)(dst+k),c1);
  			_mm_store_ps((float*)(dst+k+4),c2);
  		}
  		}	
}
void copyfloat(float* dst, const float* src, int n){
	int k=0;
#ifdef __SSE2__
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {
		for(;k+11<n;k+=12){
			__m128 a1 = _mm_load_ps((const float*)( src+k));
			__m128 a2 = _mm_load_ps((const float*)(src+k));
			__m128 a3 = _mm_load_ps((const float*)(src+k));
		
			_mm_store_ps((float*)(dst+k),a1);
			_mm_store_ps((float*)(dst+k+4),a2);
			_mm_store_ps((float*)(dst+k+8),a3);
		}
		if(k<(n-1)){
			for(;k<n;++k){
				dst[k]=(float)src[k];
			}						
        }
       }	
#endif
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void float2char(char* dst, const float* src, int n){
#ifdef HAVE_IPP
	ippsConvert_32f8s_Sfs((const Ipp32f*) float, (Ipp8s*) dst, n, ippRndFinancial, 0);  
#endif
	int k = 0;
	__m128i constant = _mm_set1_epi8(0x80);
#ifdef __SSE2__
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {    
		for( ; k+15<n;k+=16){
    		__m128 a1 = _mm_load_ps((const float*)( src+k));
  			__m128 a2 = _mm_load_ps((const float*)( src+k+4));
  		    __m128 a3 = _mm_load_ps((const float*)( src+k+8));
  		    __m128 a4 = _mm_load_ps((const float*)( src+k+12));
  		    
  		    __m128i b1 = _mm_cvtps_epi32(a1);
  			__m128i b2 = _mm_cvtps_epi32(a2);
			__m128i b3 = _mm_cvtps_epi32(a3);
  			__m128i b4 = _mm_cvtps_epi32(a4);
			
			b1 = _mm_packs_epi32(b1,b2);
			b2 = _mm_packs_epi32(b3,b4);
			b1 = _mm_packs_epi16(b1,b2);
			b1 = _mm_sub_epi8(b1,constant);		
        		_mm_storeu_si128((__m128i*)(dst+k),b1);			
			
			//_mm_store_si128((__m128i*)(z+k),b1);			
		//	_mm_store_si128((__m128i*)(z+k+4),b2);	
		//	_mm_store_si128((__m128i*)(z+k+8),b3);			
		//	_mm_store_si128((__m128i*)(z+k+12),b4);			
		}		
	}		
#endif
	if(k<n){
		for(;k<n;++k){
			dst[k]=(short)src[k];
		}						
    }		
}

void float2charScaled(char* dst, const float* src, int n, float sMin, float dRange, float sRange, float dMin){
	int k = 0;
#ifdef __SSE2__
	__m128 s_min = _mm_set1_ps(sMin);
	__m128 drange = _mm_set1_ps(dRange);
	__m128 srange = _mm_set1_ps(sRange);
	__m128 d_min = _mm_set1_ps(dMin);	
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {    
		for( ; k+7<n;k+=8){
	    		__m128 a1 = _mm_load_ps((const float*)( src+k));
  			__m128 a2 = _mm_load_ps((const float*)( src+k+4));
  		    
  		        a1 = _mm_sub_ps(a1,s_min);
        		a2 = _mm_sub_ps(a2,s_min);
        	
        		a1 = _mm_mul_ps(a1,drange);
        		a2 = _mm_mul_ps(a2,drange);
        		a1 = _mm_div_ps(a1,srange);
        		a2 = _mm_div_ps(a2,srange);
        		
        		a1 = _mm_add_ps(a1,d_min);
        		a2 = _mm_add_ps(a2,d_min);
			__m128i b1 = _mm_cvtps_epi32(a1);
			__m128i b2 = _mm_cvtps_epi32(a2);
			b1 = _mm_packs_epi32(b1,b2);
			b1 = _mm_packs_epi16(b1,b1);
			//b1 = _mm_sub_epi8(b1,_mm_set1_epi8(0x80));
			_mm_storeu_si128((__m128i*)(dst+k),b1);
		}
	
		if(k<n){
			for(;k<n;++k){
				float temp = (src[k]-sMin)*(dRange/sRange)+dMin;
				dst[k]=(char)temp;
			}						
    		}

	}else{
		for( ; k+7<n;k+=8){
	    		__m128 a1 = _mm_loadu_ps((const float*)( src+k));
  			__m128 a2 = _mm_loadu_ps((const float*)( src+k+4));
  		    
  		        a1 = _mm_sub_ps(a1,s_min);
        		a2 = _mm_sub_ps(a2,s_min);
        	
        		a1 = _mm_mul_ps(a1,drange);
        		a2 = _mm_mul_ps(a2,drange);
        		a1 = _mm_div_ps(a1,srange);
        		a2 = _mm_div_ps(a2,srange);
        		
        		a1 = _mm_add_ps(a1,d_min);
        		a2 = _mm_add_ps(a2,d_min);
			__m128i b1 = _mm_cvtps_epi32(a1);
			__m128i b2 = _mm_cvtps_epi32(a2);
			b1 = _mm_packs_epi32(b1,b2);
			b1 = _mm_packs_epi16(b1,b1);
			//b1 = _mm_sub_epi8(b1,_mm_set1_epi8(0x80));
			_mm_storeu_si128((__m128i*)(dst+k),b1);
		}
	
		if(k<n){
			for(;k<n;++k){
				float temp = (src[k]-sMin)*(dRange/sRange)+dMin;
				dst[k]=(char)temp;
			}						
    		}

	}
#endif	
}


//float2floatScaled(source, dest, size, (float) sMin, (float) dRange, (float) sRange, (float) dMin);
//float2floatScaled(source, dest, size, (float) sMin, (float) dRange, (float) sRange, (float) dMin);
void float2floatScaled(float* dst, const float* src, int n, float sMin, float dRange, float sRange, float dMin){
	int k = 0;
#ifdef __SSE2__
	__m128 s_min = _mm_set1_ps(sMin);
	__m128 drange = _mm_set1_ps(dRange);
	__m128 srange = _mm_set1_ps(sRange);
	__m128 d_min = _mm_set1_ps(dMin);	
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {    
		for( ; k+3<n;k+=4){
    		 __m128 a1 = _mm_load_ps((const float*)( src+k));	
  			a1 = _mm_sub_ps(a1,s_min);        	
        		a1 = _mm_mul_ps(a1,drange);
	        	a1 = _mm_div_ps(a1,srange);
        		a1 = _mm_add_ps(a1,d_min);
			_mm_store_ps((float*)(dst+k),a1);
		}

		if(k<n){
			for(;k<n;++k){
				float temp = (src[k]-sMin)*(dRange/sRange)+dMin;
				dst[k]=temp;
			}						
    		}
	}else{
		for( ; k+3<n;k+=4){
    		 __m128 a1 = _mm_loadu_ps((const float*)( src+k));	
  			a1 = _mm_sub_ps(a1,s_min);        	
        		a1 = _mm_mul_ps(a1,drange);
	        	a1 = _mm_div_ps(a1,srange);
        		a1 = _mm_add_ps(a1,d_min);
			_mm_storeu_ps((float*)(dst+k),a1);
		}

		if(k<n){
			for(;k<n;++k){
				float temp = (src[k]-sMin)*(dRange/sRange)+dMin;
				dst[k]=temp;
			}						
    		}

	}
#endif
}



void float2uchar(unsigned char* dst, const float* src, int n){
#ifdef HAVE_IPP
	ippsConvert_32f8u_Sfs((const Ipp32f*) float, (Ipp8u*) dst, n, ippRndFinancial, 0);  
#endif
	int k = 0;
#ifdef __SSE2__
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {    
		for( ; k+15<n;k+=16){
    		__m128 a1 = _mm_load_ps((const float*)( src+k));
  			__m128 a2 = _mm_load_ps((const float*)( src+k+4));
  		    __m128 a3 = _mm_load_ps((const float*)( src+k+8));
  		    __m128 a4 = _mm_load_ps((const float*)( src+k+12));
  		    __m128i b1 = _mm_cvtps_epi32(a1);
  			__m128i b2 = _mm_cvtps_epi32(a2);
			__m128i b3 = _mm_cvtps_epi32(a3);
  			__m128i b4 = _mm_cvtps_epi32(a4);
			b1 = _mm_packs_epi32(b1,b2);
			b2 = _mm_packs_epi32(b3,b4);
			b1 = _mm_packus_epi16(b1,b2);		
        	_mm_storeu_si128((__m128i*)(dst+k),b1);			
		}		
	}		
#endif
	if(k<n){
		for(;k<n;++k){
			dst[k]=(unsigned char)src[k];
		}						
    }		
}


void float2ucharScaled(unsigned char* dst,const  float* src, int n, float sMin, float dRange, float sRange, float dMin){
	int k = 0;
#ifdef __SSE2__
	__m128 s_min = _mm_set1_ps(sMin);
	__m128 drange = _mm_set1_ps(dRange);
	__m128 srange = _mm_set1_ps(sRange);
	__m128 d_min = _mm_set1_ps(dMin);	
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {    
		for( ; k+7<n;k+=8){
    			__m128 a1 = _mm_load_ps((const float*)( src+k));
  			__m128 a2 = _mm_load_ps((const float*)( src+k+4));
  		    	a1 = _mm_sub_ps(a1,s_min);
        		a2 = _mm_sub_ps(a2,s_min);
        		a1 = _mm_mul_ps(a1,drange);
        		a2 = _mm_mul_ps(a2,drange);
        	a1 = _mm_div_ps(a1,srange);
        	a2 = _mm_div_ps(a2,srange);
        	a1 = _mm_add_ps(a1,d_min);
        	a2 = _mm_add_ps(a2,d_min);
		__m128i b1 = _mm_cvtps_epi32(a1);
		__m128i b2 = _mm_cvtps_epi32(a2);
		b1 = _mm_packs_epi32(b1,b2);
		b1 = _mm_packus_epi16(b1,b1);
		_mm_storeu_si128((__m128i*)(dst+k),b1);
		}
		if(k<n){
			for(;k<n;++k){
				float temp = (src[k]-sMin)*(dRange/sRange)+dMin;
				dst[k]=(unsigned char)temp;
			}							
    		}
	}else{
		for( ; k+7<n;k+=8){
    		__m128 a1 = _mm_loadu_ps((const float*)( src+k));
  			__m128 a2 = _mm_loadu_ps((const float*)( src+k+4));
  		    a1 = _mm_sub_ps(a1,s_min);
        	a2 = _mm_sub_ps(a2,s_min);
        	a1 = _mm_mul_ps(a1,drange);
        	a2 = _mm_mul_ps(a2,drange);
        	a1 = _mm_div_ps(a1,srange);
        	a2 = _mm_div_ps(a2,srange);
        	a1 = _mm_add_ps(a1,d_min);
        	a2 = _mm_add_ps(a2,d_min);
		__m128i b1 = _mm_cvtps_epi32(a1);
		__m128i b2 = _mm_cvtps_epi32(a2);
		b1 = _mm_packs_epi32(b1,b2);
		b1 = _mm_packus_epi16(b1,b1);
		_mm_storeu_si128((__m128i*)(dst+k),b1);
		}
		if(k<n){
			for(;k<n;++k){
				float temp = (src[k]-sMin)*(dRange/sRange)+dMin;
				dst[k]=(unsigned char)temp;
			}							
    		}
	
	
	}
#endif
}




void float2short(short* dst, const float* src, int n){
#ifdef HAVE_IPP_AND_USE_ALWAYS
	(void)ippsConvert_32f16s_Sfs((const Ipp32f*) src, (Ipp16s*) dst, n, ippRndFinancial, 0);
#else
int k=0;
#ifdef __SSE2__   	
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {    
	for( ; k+7<n;k+=8){
    		__m128 a1 = _mm_load_ps((const float*)( src+k));
  			__m128 a2 = _mm_load_ps((const float*)( src+k+4));
  		    __m128i b1 = _mm_cvtps_epi32(a1);
  			__m128i b2 = _mm_cvtps_epi32(a2);
			b1 = _mm_packs_epi32(b1,b2);		
        	_mm_store_si128((__m128i*)(dst+k),b1);
        }

  		if(k<(n-1)){
			for(;k<n;++k){
				dst[k]=(short)src[k];
			}						
        }
  	}	
#else
	for(;k<n;++k){
		dst[k]=(short)src[k];
	}
#endif
#endif
}


void float2shortScaled(short* dst, const float* src, int n, float sMin, float dRange, float sRange, float dMin){
int k=0;
#ifdef __SSE2__
	__m128 s_min = _mm_set1_ps(sMin);
	__m128 drange = _mm_set1_ps(dRange);
	__m128 srange = _mm_set1_ps(sRange);
	__m128 d_min = _mm_set1_ps(dMin);	
   	
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {    
	for( ; k+7<n;k+=8){
    		__m128 a1 = _mm_load_ps((const float*)( src+k));
  			__m128 a2 = _mm_load_ps((const float*)( src+k+4));
  		    
			a1 = _mm_sub_ps(a1,s_min);
        	a2 = _mm_sub_ps(a2,s_min);
        	a1 = _mm_mul_ps(a1,drange);
        	a2 = _mm_mul_ps(a2,drange);
        	a1 = _mm_div_ps(a1,srange);
        	a2 = _mm_div_ps(a2,srange);
        	a1 = _mm_add_ps(a1,d_min);
        	a2 = _mm_add_ps(a2,d_min);

			__m128i b1 = _mm_cvtps_epi32(a1);
  			__m128i b2 = _mm_cvtps_epi32(a2);
			b1 = _mm_packs_epi32(b1,b2);			
			_mm_store_si128((__m128i*)(dst+k),b1);
        	}

  		if(k<(n-1)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]=(short)temp;
			}						
        	}
  	}else{
	for( ; k+7<n;k+=8){
    		__m128 a1 = _mm_loadu_ps((const float*)( src+k));
  			__m128 a2 = _mm_loadu_ps((const float*)( src+k+4));
  		    
			a1 = _mm_sub_ps(a1,s_min);
        	a2 = _mm_sub_ps(a2,s_min);
        	a1 = _mm_mul_ps(a1,drange);
        	a2 = _mm_mul_ps(a2,drange);
        	a1 = _mm_div_ps(a1,srange);
        	a2 = _mm_div_ps(a2,srange);
        	a1 = _mm_add_ps(a1,d_min);
        	a2 = _mm_add_ps(a2,d_min);

			__m128i b1 = _mm_cvtps_epi32(a1);
  			__m128i b2 = _mm_cvtps_epi32(a2);
			b1 = _mm_packs_epi32(b1,b2);			
			_mm_storeu_si128((__m128i*)(dst+k),b1);
        	}

  		if(k<(n-1)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]=(short)temp;
			}						
        	}
	}	

#endif
}




void float2ushort(unsigned short* dst, const float* src, int n){
#ifdef HAVE_IPP_AND_USE_ALWAYS
	(void)ippsConvert_32f16u_Sfs((const Ipp32f*) src, (Ipp16u*) dst, n, ippRndFinancial, 0);
#else
int k=0;
#ifdef __SSE2__   	
	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {    
	for( ; k+7<n;k+=8){
    		__m128 a1 = _mm_load_ps((const float*)( src+k));
  			__m128 a2 = _mm_load_ps((const float*)( src+k+4));
			__m128i b1 = _mm_cvtps_epi32(a1);
  			__m128i b2 = _mm_cvtps_epi32(a2);
#ifdef __SSE4_1__
			_mm_packus_epi32(b1,b2);
#else
			b1 = _mm_sub_epi32(b1,_mm_set1_epi32(0x8000));
			b2 = _mm_sub_epi32(b2,_mm_set1_epi32(0x8000));
			b1 = _mm_packs_epi32(b1,b2);
			b1 = _mm_xor_si128(b1,_mm_set1_epi16(0x8000));
#endif
			_mm_store_si128((__m128i*)(dst+k),b1);
        }

  		if(k<(n-1)){
			for(;k<n;++k){
				dst[k]=(unsigned short)src[k];
			}						
        }
  	}	
#endif
#endif

}

void float2ushortScaled(unsigned short* dst, const float* src, int n, float sMin, float dRange, float sRange, float dMin){
int k=0;
#ifdef __SSE2__   	
	__m128 s_min = _mm_set1_ps(sMin);
	__m128 drange = _mm_set1_ps(dRange);
	__m128 srange = _mm_set1_ps(sRange);
	__m128 d_min = _mm_set1_ps(dMin);	

	if ( ALIGNED2(src,dst) && stack_simd_aligned() ) {    
	for( ; k+7<n;k+=8){
    		__m128 a1 = _mm_load_ps((const float*)( src+k));
  		__m128 a2 = _mm_load_ps((const float*)( src+k+4));
                a1 = _mm_sub_ps(a1,s_min);
        	a2 = _mm_sub_ps(a2,s_min);
        	a1 = _mm_mul_ps(a1,drange);
        	a2 = _mm_mul_ps(a2,drange);
        	a1 = _mm_div_ps(a1,srange);
        	a2 = _mm_div_ps(a2,srange);
        	a1 = _mm_add_ps(a1,d_min);
        	a2 = _mm_add_ps(a2,d_min);
		__m128i b1 = _mm_cvtps_epi32(a1);
		__m128i b2 = _mm_cvtps_epi32(a2);
#ifdef __SSE4_1__
		_mm_packus_epi32(b1,b2);
#else
		b1 = _mm_sub_epi32(b1,_mm_set1_epi32(0x8000));
		b2 = _mm_sub_epi32(b2,_mm_set1_epi32(0x8000));
		b1 = _mm_packs_epi32(b1,b2);
		b1 = _mm_xor_si128(b1,_mm_set1_epi16(0x8000));
#endif			        	
        	_mm_store_si128((__m128i*)(dst+k),b1);
        }

  		if(k<(n-1)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]=(unsigned short)temp;
			}						
        }
  	}else{
for( ; k+7<n;k+=8){
    		__m128 a1 = _mm_loadu_ps((const float*)( src+k));
  		__m128 a2 = _mm_loadu_ps((const float*)( src+k+4));
                a1 = _mm_sub_ps(a1,s_min);
        	a2 = _mm_sub_ps(a2,s_min);
        	a1 = _mm_mul_ps(a1,drange);
        	a2 = _mm_mul_ps(a2,drange);
        	a1 = _mm_div_ps(a1,srange);
        	a2 = _mm_div_ps(a2,srange);
        	a1 = _mm_add_ps(a1,d_min);
        	a2 = _mm_add_ps(a2,d_min);
		__m128i b1 = _mm_cvtps_epi32(a1);
		__m128i b2 = _mm_cvtps_epi32(a2);
#ifdef __SSE4_1__
		_mm_packus_epi32(b1,b2);
#else
		b1 = _mm_sub_epi32(b1,_mm_set1_epi32(0x8000));
		b2 = _mm_sub_epi32(b2,_mm_set1_epi32(0x8000));
		b1 = _mm_packs_epi32(b1,b2);
		b1 = _mm_xor_si128(b1,_mm_set1_epi16(0x8000));
#endif			        	
        	_mm_storeu_si128((__m128i*)(dst+k),b1);
        	}

  		if(k<(n-1)){
			for(;k<n;++k){
				float temp = ((src[k]-sMin)*(dRange/sRange))+dMin;
				dst[k]=(unsigned short)temp;
			}						
          	}
	}	
#endif
}


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void interleaveFloat(float* rc, const float *r, const float *c, int n){
#ifdef HAVE_IPP
	(void)ippsRealToCplx_32f((const Ipp32f*) r,(const Ipp32f*) c, (Ipp32fc*)rc, elements);
#else
	int k=0;
	int offset = 0;
	if ( ALIGNED3(rc,r,c) && stack_simd_aligned() ) {   
#ifdef __SSE2__
		for( ; k+3<n;k+=4){
    		__m128 a1 = _mm_load_ps((const float*)( r+k));
  			__m128 b1 = _mm_load_ps((const float*)( c+k));
  			//__m128 a2 = _mm_load_ps((const float*)( r+k+4));
  			//__m128 b2 = _mm_load_ps((const float*)( c+k+4));
  			
  			__m128 c1 = _mm_unpacklo_ps(a1,b1);
  			__m128 c2 = _mm_unpackhi_ps(a1,b1); 	
			//__m128 c3 = _mm_unpacklo_ps(a2,b2);
  			//__m128 c4 = _mm_unpackhi_ps(a2,b2); 	
			
			_mm_store_ps((float*)(rc+k),c1);
        	_mm_store_ps((float*)(rc+k+4),c2);
 			//_mm_store_ps((float*)(rc+k+8),c3);
 			//_mm_store_ps((float*)(rc+k+12),c4);
 		}
        	
        if (k<(n-1)){
     		int windex = k;
     		if (k!=0){
				windex = 2*k;
			}
        	for(;k<n;k++){
				//printf(" k %i index %i \n",k,windex);
				rc[windex]=(short)r[k];
				rc[windex+1]=(short)c[k];
				windex+=2;
			}						
        }
    }
#else
	for(;k+1<n;k+=2){
		rc[k] = r[k];
		rc[k+1] = c[k];
	}
#endif
#endif
}
  void mul_const(float* c, const float *a, const float b, int n){
#ifdef HAVE_IPP
	//(void)ippsRealToCplx_32f((const Ipp32f*) r,(const Ipp32f*) c, (Ipp32fc*)rc, elements);
#else
	int k=0;
	__m128 b1 = _mm_load1_ps(&b);
	if ( ALIGNED2(c,a) && stack_simd_aligned() ) {   
#ifdef __SSE2__
		for( ; k+3<n;k+=4){
    		__m128 a1 = _mm_load_ps((const float*)(a+k));
  			a1 = _mm_mul_ps(a1,b1);
			_mm_store_ps((float*)(c+k),a1);
        }
    }
 #endif       	
	if (k<(n-1)){
		for(;k+1<n;k+=2){
			c[k] = a[k] * b;
		}
	}
#endif
}
