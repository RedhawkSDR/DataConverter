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

/* 
 * File:   DATATYPETRANSFORM.H
 * Author: 
 *
 * Created on June 1, 2011, 1:19 PM
 */

#ifndef _DATATYPETRANSFORMOPT_H
#define	_DATATYPETRANSFORMOPT_H

#include <iostream>
#include <typeinfo>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <limits>
#include <numeric>
#include <complex>
#include <vector>
#include <iostream>
#include <algorithm>
#include "mathOptimizations.h"

namespace dataTypeTransformOpt {

    /**********
     Data Limits
     **********/

    // Return the largest value for the max or abs(min)

    template <typename TYPE> double getNumericalLimits(TYPE val) {
        return std::max((double) - 1 * std::numeric_limits<TYPE>::min(), (double) (std::numeric_limits<TYPE>::max)());
    }

    template <typename TYPE> double getNumericalLimits(std::complex<TYPE> val) {
        return std::max((double) - 1 * std::numeric_limits<TYPE>::min(), (double) (std::numeric_limits<TYPE>::max)());
    }

    // Quick tests to see if data is unsigned

    template <typename TYPE> bool isUnsigned(TYPE val) {
        return ((std::numeric_limits<TYPE>::min)() == 0);
    }

    /**********
     Conversions
     **********/


    // Performs Max Scale conversions with float/normalization between [-1,1]

    template <typename IN_TYPE, typename OUT_TYPE> OUT_TYPE convertTypeScale(IN_TYPE ret, bool normalize = true) {
        double dRange = double(std::numeric_limits<OUT_TYPE>::max()) - double(std::numeric_limits<OUT_TYPE>::min());
        double dMin = double(std::numeric_limits<OUT_TYPE>::min());
        double sRange = double(std::numeric_limits<IN_TYPE>::max()) - double(std::numeric_limits<IN_TYPE>::min());
        double sMin = double(std::numeric_limits<IN_TYPE>::min());

        // Normalize float/double output b/t -1 and 1
        if (normalize && (typeid (OUT_TYPE) == typeid (float) || typeid (OUT_TYPE) == typeid (double))) {
            dRange = 2;
            dMin = -1;

        }

        // Normalize float/double input b/t -1 and 1
        if (normalize && (typeid (IN_TYPE) == typeid (float) || typeid (IN_TYPE) == typeid (double))) {
            sRange = 2;
            sMin = -1;
        }

        //Casting loses precision
        double tmpFloat = (ret - sMin)* (dRange / sRange) + dMin;
        return OUT_TYPE(tmpFloat);
    }

    template <typename IN_TYPE, typename IN_TYPE_ALLOC, typename OUT_TYPE> void convertVectorDataType(std::vector<IN_TYPE, IN_TYPE_ALLOC> *source, std::vector<OUT_TYPE> *dest, bool normalize = true) {
        //Clear Output Vector
        dest->resize(source->size());
        if (!source->empty())
            convertDataType(&source->at(0), &dest->at(0), true, normalize, normalize, source->size());
    }

    template <typename IN_TYPE, typename OUT_TYPE> void convertDataType(IN_TYPE* source, OUT_TYPE* dest, bool scale = true, bool in_normalized = true, bool out_normalized = true, size_t size = 10000) {

        double dRange = double(std::numeric_limits<OUT_TYPE>::max()) - double(std::numeric_limits<OUT_TYPE>::min());
        double dMin = double(std::numeric_limits<OUT_TYPE>::min());
        double sRange = double(std::numeric_limits<IN_TYPE>::max()) - double(std::numeric_limits<IN_TYPE>::min());
        double sMin = double(std::numeric_limits<IN_TYPE>::min());
        if (in_normalized && (typeid (IN_TYPE) == typeid (float) || typeid (IN_TYPE) == typeid (double))) {
            sRange = 2;
            sMin = -1;
        }
        if (out_normalized && (typeid (OUT_TYPE) == typeid (float) || typeid (OUT_TYPE) == typeid (double))) {
            dRange = 2;
            dMin = -1;
        }

        if (typeid (OUT_TYPE) == typeid (float) && (!out_normalized)) {
            dRange = sRange;
            dMin = sMin;
        }

        if (!scale) {
            //////////////////////////CHAR/////////////////////////////
            if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (char)) {
                char2uchar((unsigned char*) &(dest[0]), (const char*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (char)) {
                char2short((short*) &(dest[0]), (const char*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (char)) {
                char2ushort((unsigned short*) &(dest[0]), (const char*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (char)) {
                char2float((float*) &(dest[0]), (const char*) &(source[0]), size);
            }                //unsigned char
            else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2char((char*) &(dest[0]), (const unsigned char*) &(source[0]), size);
            }                //doesnt exist
            else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2short((short*) &(dest[0]), (const unsigned char*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2ushort((unsigned short*) &(dest[0]), (const unsigned char*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2float((float*) &(dest[0]), (const unsigned char*) &(source[0]), size);
            }                //////////////////////////SHORT////////////////////////////////
            else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (short)) {
                short2ushort((unsigned short*) &(dest[0]), (const short*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (short)) {
                short2char((char*) &(dest[0]), (const short*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid ( short)) {
                short2uchar((unsigned char*) &(dest[0]), (short*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (short)) {
                short2float((float*) &(dest[0]), (const short*) &(source[0]), size);
            }                //unsigned short
                //else if (typeid (OUT_TYPE) == typeid( unsigned char) && typeid (IN_TYPE) == typeid(unsigned short)){
                //	 ushort2uchar((unsigned char*)&(dest[0]), (const unsigned short*)&(source[0]),size);
                //}
            else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (short)) {
                short2char((char*) &(dest[0]), (const short*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (unsigned short)) {
                ushort2short((short*) &(dest[0]), (const unsigned short*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (unsigned short)) {
                ushort2float((float*) &(dest[0]), (const unsigned short*) &(source[0]), size);
            }
                ///////////////////////////FLOAT///////////////////////////////	


                /* unscaled but normalized float is a useless function ignore all calls to it
	
                        else if	(typeid (OUT_TYPE) == typeid(short) && typeid (IN_TYPE) == typeid(float) ){
                                 float2short((short*)&(dest[0]), (const float*)&(source[0]),size);
                        }
                        else if (typeid (OUT_TYPE) == typeid(unsigned short) && typeid (IN_TYPE) == typeid(float)){
                                 float2ushort((unsigned short*)&(dest[0]), (const float*)&(source[0]),size);
                        }
                        else if (typeid (OUT_TYPE) == typeid(char) && typeid (IN_TYPE) == typeid(float)){
                                 float2char((char*)&(dest[0]), (const float*)&(source[0]),size);	
                        }
                        else if (typeid (OUT_TYPE) == typeid(unsigned char) && typeid (IN_TYPE) == typeid(float)){
                                 float2uchar((unsigned char*)&(dest[0]), (const float*)&(source[0]),size);	
                        }
                 */
            else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (float)) {
                std::cerr << "Ignoring Request - will scale the output to produce a valid range" << std::endl;
                float2shortScaled((short*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (float)) {
                std::cerr << "Ignoring Request - will scale the output to produce a valid range" << std::endl;
                float2ushortScaled((unsigned short*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (float)) {
                std::cerr << "Ignoring Request - will scale the output to produce a valid range" << std::endl;
                float2charScaled((char*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (float)) {
                std::cerr << "Ignoring Request - will scale the output to produce a valid range" << std::endl;
                float2ucharScaled((unsigned char*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else {
                std::cerr << "Ignoring Request - will scale the output to produce a valid range" << std::endl;
                double tmpFloat;
                for (size_t i = 0; i < (size_t) size; i++) {
                    tmpFloat = (source[i] - sMin)* (dRange / sRange) + dMin;
                    dest[i] = OUT_TYPE(tmpFloat);
                }
            }

        } else {
            //char type

            if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (char)) {
                char2ucharScaled((unsigned char*) &(dest[0]), (const char*) &(source[0]), size);
            } else if ((typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (char))) {
                char2shortScaled((short*) &(dest[0]), (const char*) &(source[0]), size, (short) sMin, (short) (dRange / sRange), (short) dMin);
            } else if ((typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (char))) {
                char2ushortScaled((unsigned short*) &(dest[0]), (const char*) &(source[0]), size, (short) sMin, (short) (dRange / sRange), (short) dMin);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (char)) {
                char2floatScaled((float*) &(dest[0]), (const char*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2charScaled((char*) &(dest[0]), (const unsigned char*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2shortScaled((short*) &(dest[0]), (unsigned char*) &(source[0]), (short) size, (short) sMin, (short) (dRange / sRange), (short) dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2ushortScaled((unsigned short*) &(dest[0]), (unsigned char*) &(source[0]), size, (short) sMin, (short) (dRange / sRange), (short) dMin);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2floatScaled((float*) &(dest[0]), (const unsigned char*) &(source[0]), size, sMin, dRange, sRange, dMin);
            }
                //////////////////////////////SHORT SCALED////////////////////////////
            else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (short)) {
                short2charScaled((char*) &(dest[0]), (const short*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (short)) {
                short2ucharScaled((unsigned char*) &(dest[0]), (const short*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (short)) {
                short2ushortScaled((unsigned short*) &(dest[0]), (const short*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (unsigned short)) {
                ushort2shortScaled((short*) &(dest[0]), (const unsigned short*) &(source[0]), size);
            }
            else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (unsigned short)) {
                ushort2charScaled((char*) &(dest[0]), (const unsigned short*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (unsigned short)) {
                ushort2ucharScaled((unsigned char*) &(dest[0]), (const unsigned short*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (short)) {
                short2floatScaled((float*) &(dest[0]), (const short*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (unsigned short)) {
                ushort2floatScaled((float*) &(dest[0]), (const unsigned short*) &(source[0]), size, sMin, dRange, sRange, dMin);
            }                //////////////////////////////FLOAT SCALED////////////////////////////
            else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (float)) {
                float2shortScaled((short*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (float)) {
                float2ushortScaled((unsigned short*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (float)) {
                float2charScaled((char*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (float)) {
                float2ucharScaled((unsigned char*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (float)) {
                float2floatScaled((float*) &dest[0], (const float*) &source[0], size, sMin, dRange, sRange, dMin);
            }
            else {

                //Clear Output Vector
                double tmpFloat;
                for (size_t i = 0; i < (size_t) size; i++) {
                    tmpFloat = (source[i] - sMin)* (dRange / sRange) + dMin;
                    dest[i] = OUT_TYPE(tmpFloat);
                }
            }
        }
    }

    // Bounds given by user

    template <typename IN_TYPE, typename IN_TYPE_ALLOC, typename OUT_TYPE> void convertVectorDataType(std::vector<IN_TYPE, IN_TYPE_ALLOC> *source, std::vector<OUT_TYPE> *dest, double sMin, double sMax, double dMin, double dMax) {
        //Clear Output Vector
        dest->resize(source->size());
        if (!source->empty())
            convertDataType(&source->at(0), &dest->at(0), sMin, sMax, dMin, dMax, source->size());
    }

    template <typename IN_TYPE, typename OUT_TYPE> void convertDataType(IN_TYPE *source, OUT_TYPE *dest, double sMin, double sMax, double dMin, double dMax, size_t size) {
        double dRange = dMax - dMin;
        double sRange = sMax - sMin;


        if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (char)) {
            char2shortScaled((short*) &(dest[0]), (const char*) &(source[0]), source->size(), sMin, dRange, sRange, dMin);
        }            //else if	(typeid (OUT_TYPE) == typeid(unsigned short) && typeid (IN_TYPE) === typeid(unsigned char)){
            //	uchar2shortScaled((short*)&(dest[0]), (unsigned char*)&(source[0]),source->size(), sMin, dRange,sRange,dMin);
            //}
        else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (char)) {
            char2ushortScaled((short*) &(dest[0]), (const char*) &(source[0]), source->size(), sMin, dRange, sRange, dMin);
        }            //else if	(typeid (OUT_TYPE) == typeid(unsigned short) && typeid (IN_TYPE) === typeid(unsigned char)){
            //	uchar2ushortScaled((short*)&(dest[0]), (unsigned char*)&(source[0]),source->size(), sMin, dRange,sRange,dMin);
            //}
        else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (char)) {
            char2floatScaled((float*) &(dest[0]), (const char*) &(source[0]), source->size(), sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (unsigned char)) {
            uchar2floatScaled((float*) &(dest[0]), (const unsigned char*) &(source[0]), source->size(), sMin, dRange, sRange, dMin);
        }            //////////////////////////////SHORT SCALED////////////////////////////
        else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (short)) {
            short2charScaled((char*) &(dest[0]), (const short*) &(source[0]), source->size());
        } else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (unsigned short)) {
            ushort2charScaled((char*) &(dest[0]), (const unsigned short*) &(source[0]), source->size());
        }            //else if (typeid (OUT_TYPE) == typeid(unsigned char) && typeid (IN_TYPE) == typeid(short){
            //	short2ucharS((unsigned char*)&(dest[0]), (short*)&(source[0]), source->size());
            //}
        else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (short)) {
            short2floatScaled((float*) &(dest[0]), (const short*) &(source[0]), source->size(), sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (unsigned short)) {
            ushort2floatScaled((float*) &(dest[0]), (const unsigned short*) &(source[0]), source->size(), sMin, dRange, sRange, dMin);
        }            //////////////////////////////FLOAT SCALED////////////////////////////
        else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (float)) {
            float2shortScaled((short*) &(dest[0]), (const float*) &(source[0]), source->size(), sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (float)) {
            float2ushortScaled((unsigned short*) &(dest[0]), (const float*) &(source[0]), source->size(), sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (float)) {
            float2charScaled((char*) &(dest[0]), (const float*) &(source[0]), source->size(), sMin, dRange, sRange, dMin);
        }            //else if (typeid (OUT_TYPE) == typeid(unsigned char) && typeid (IN_TYPE) == typeid(float)){
            //	float2ucharS((unsigned char*)&(dest[0]), (float*)(source[0]),source->size());	
            //}


        else {
            double tmpFloat;
            for (size_t i = 0; i < source->size(); i++) {
                if (source[i] <= sMin)
                    tmpFloat = dMin;
                else if (source[i] >= sMax)
                    tmpFloat = dMax;
                else
                    tmpFloat = (source[i] - sMin)* (dRange / sRange) + dMin;
                dest->push_back(OUT_TYPE(tmpFloat));
            }

        }

    }

    template <typename IN_TYPE, typename OUT_TYPE> void convertDataTypeRange(IN_TYPE* source, OUT_TYPE* dest, double sMin, double sRange, double dMin, double dRange, size_t size = 10000) {
        if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (char)) {
            char2ucharScaled((unsigned char*) &(dest[0]), (const char*) &(source[0]), size);
        } else if ((typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (char))) {
            char2shortScaled((short*) &(dest[0]), (const char*) &(source[0]), size, (short) sMin, (short) (dRange / sRange), (short) dMin);
        } else if ((typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (char))) {
            char2ushortScaled((unsigned short*) &(dest[0]), (const char*) &(source[0]), size, (short) sMin, (short) (dRange / sRange), (short) dMin);
        } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (char)) {
            char2floatScaled((float*) &(dest[0]), (const char*) &(source[0]), size, sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (unsigned char)) {
            uchar2charScaled((char*) &(dest[0]), (const unsigned char*) &(source[0]), size);
        } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (unsigned char)) {
            uchar2shortScaled((short*) &(dest[0]), (unsigned char*) &(source[0]), (short) size, (short) sMin, (short) (dRange / sRange), (short) dMin);
        } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (unsigned char)) {
            uchar2ushortScaled((unsigned short*) &(dest[0]), (unsigned char*) &(source[0]), size, (short) sMin, (short) (dRange / sRange), (short) dMin);
        } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (unsigned char)) {
            uchar2floatScaled((float*) &(dest[0]), (const unsigned char*) &(source[0]), size, sMin, dRange, sRange, dMin);
        }
            //////////////////////////////SHORT SCALED////////////////////////////
        else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (short)) {
            short2charScaled((char*) &(dest[0]), (const short*) &(source[0]), size, sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (short)) {
            short2ucharScaled((unsigned char*) &(dest[0]), (const short*) &(source[0]), size, sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (short)) {
            short2ushortScaled((unsigned short*) &(dest[0]), (const short*) &(source[0]), size);
        } else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (unsigned short)) {
            ushort2shortScaled((short*) &(dest[0]), (const unsigned short*) &(source[0]), size);
        }
        else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (unsigned short)) {
            ushort2charScaled((char*) &(dest[0]), (const unsigned short*) &(source[0]), size, sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (unsigned short)) {
            ushort2ucharScaled((unsigned char*) &(dest[0]), (const unsigned short*) &(source[0]), size, sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (short)) {
            short2floatScaled((float*) &(dest[0]), (const short*) &(source[0]), size, sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (unsigned short)) {
            ushort2floatScaled((float*) &(dest[0]), (const unsigned short*) &(source[0]), size, sMin, dRange, sRange, dMin);
        }            //////////////////////////////FLOAT SCALED////////////////////////////
        else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (float)) {
            float2shortScaled((short*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (float)) {
            float2ushortScaled((unsigned short*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (float)) {
            float2charScaled((char*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (float)) {
            float2ucharScaled((unsigned char*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (float)) {
            float2floatScaled((float*) &dest[0], (const float*) &source[0], size, sMin, dRange, sRange, dMin);
        }
        else {
            //printf("Failed all checks, default action\n");
            //Clear Output Vector
            double tmpFloat;
            for (size_t i = 0; i < (size_t) size; i++) {
                tmpFloat = (source[i] - sMin)* (dRange / sRange) + dMin;
                dest[i] = OUT_TYPE(tmpFloat);
            }
        }
    }





}



#endif	/* _DATATYPETRANSFORM_H */
