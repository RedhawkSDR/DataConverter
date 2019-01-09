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
#define _DATATYPETRANSFORMOPT_H

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
#include "ossie/debug.h"

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

    template <typename IN_TYPE, typename OUT_TYPE>
    void convertDataType(IN_TYPE* source, OUT_TYPE* dest, bool scale = true, bool in_normalized = true, bool out_normalized = true, size_t size = 10000, LOGGER _log=LOGGER()) {

        if (!_log) {
            _log = rh_logger::Logger::getLogger("DataConverter_dataTypeTransformOpt");
            RH_DEBUG(_log, "convertDataType method passed null logger; creating logger "<<_log->getName());
        } else {
            RH_DEBUG(_log, "convertDataType method passed valid logger "<<_log->getName());
        }

        double sRange = double(std::numeric_limits<IN_TYPE>::max()) - double(std::numeric_limits<IN_TYPE>::min());
        double sMin = double(std::numeric_limits<IN_TYPE>::min());
        double dRange = double(std::numeric_limits<OUT_TYPE>::max()) - double(std::numeric_limits<OUT_TYPE>::min());
        double dMin = double(std::numeric_limits<OUT_TYPE>::min());

        if (in_normalized && (typeid (IN_TYPE) == typeid (float) || typeid (IN_TYPE) == typeid (double))) {
            sRange = 2;
            sMin = -1;
        } else if (typeid (IN_TYPE) == typeid (float) && scale) {
            RH_WARN(_log,"convertDataType| Non-normalized float input - using full numeric limits of float for input range");
            sRange = double(std::numeric_limits<IN_TYPE>::max())*2;
            sMin = double(std::numeric_limits<IN_TYPE>::max())*-1;
        } else if (typeid (IN_TYPE) == typeid (double) && scale) {
            RH_WARN(_log,"convertDataType| Non-normalized double input - using 1/2 of full double range for input range (centered at 0)");
            // cannot represent full range of double as a double, since it's 2*max_val
            sRange = double(std::numeric_limits<IN_TYPE>::max());
            sMin = double((std::numeric_limits<IN_TYPE>::max()/2))*-1;
        }
        if (typeid (OUT_TYPE) == typeid (float) || typeid (OUT_TYPE) == typeid (double)) {
            if (out_normalized) {
                dRange = 2;
                dMin = -1;
            } else {
                dRange = sRange;
                dMin = sMin;
            }
        }

        if (!scale) {
            //////////////////////////////CHAR////////////////////////////
            if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (signed char)) {
                char2uchar((unsigned char*) &(dest[0]), (const char*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (signed char)) {
                char2short((short*) &(dest[0]), (const char*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (signed char)) {
                char2ushort((unsigned short*) &(dest[0]), (const char*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (signed char)) {
                char2float((float*) &(dest[0]), (const char*) &(source[0]), size);
            }
            //////////////////////////UNSIGNED CHAR///////////////////////
            else if (typeid (OUT_TYPE) == typeid (signed char) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2char((char*) &(dest[0]), (const unsigned char*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2short((short*) &(dest[0]), (const unsigned char*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2ushort((unsigned short*) &(dest[0]), (const unsigned char*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2float((float*) &(dest[0]), (const unsigned char*) &(source[0]), size);
            }
            //////////////////////////////SHORT////////////////////////////
            else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (short)) {
                short2ushort((unsigned short*) &(dest[0]), (const short*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (signed char) && typeid (IN_TYPE) == typeid (short)) {
                short2char((char*) &(dest[0]), (const short*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid ( short)) {
                short2uchar((unsigned char*) &(dest[0]), (const short*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (short)) {
                short2float((float*) &(dest[0]), (const short*) &(source[0]), size);
            }
            //////////////////////////UNSIGNED SHORT///////////////////////
            else if (typeid (OUT_TYPE) == typeid( unsigned char) && typeid (IN_TYPE) == typeid(unsigned short)){
                ushort2uchar((unsigned char*)&(dest[0]), (const unsigned short*)&(source[0]),size);
            } else if (typeid (OUT_TYPE) == typeid (signed char) && typeid (IN_TYPE) == typeid (unsigned short)) {
                ushort2char((char*) &(dest[0]), (const unsigned short*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (unsigned short)) {
                ushort2short((short*) &(dest[0]), (const unsigned short*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (unsigned short)) {
                ushort2float((float*) &(dest[0]), (const unsigned short*) &(source[0]), size);
            }
            ///////////////////////////FLOAT///////////////////////////////
            // Note: unscaled but normalized float is a useless function ignore all calls to it
            else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (float)) {
                if (in_normalized) {
                    RH_WARN(_log,"convertDataType| Ignoring request not to scale; Scaling normalized float conversion to integral type to produce a valid range");
                    float2shortScaled((short*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
                } else {
                    RH_DEBUG(_log,"convertDataType| Requested no scaling for non-normalized float conversion to integral type");
                    float2short((short*)&(dest[0]), (const float*)&(source[0]),size);
                }
            } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (float)) {
                if (in_normalized) {
                    RH_WARN(_log,"convertDataType| Ignoring request not to scale; Scaling normalized float conversion to integral type to produce a valid range");
                    float2ushortScaled((unsigned short*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
                } else {
                    RH_DEBUG(_log,"convertDataType| Requested no scaling for non-normalized float conversion to integral type");
                    float2ushort((unsigned short*)&(dest[0]), (const float*)&(source[0]),size);
                }
            } else if (typeid (OUT_TYPE) == typeid (signed char) && typeid (IN_TYPE) == typeid (float)) {
                if (in_normalized) {
                    RH_WARN(_log,"convertDataType| Ignoring request not to scale; Scaling normalized float conversion to integral type to produce a valid range");
                    float2charScaled((char*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
                } else {
                    RH_DEBUG(_log,"convertDataType| Requested no scaling for non-normalized float conversion to integral type");
                    float2char((char*)&(dest[0]), (const float*)&(source[0]),size);
                }
            } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (float)) {
                if (in_normalized) {
                    RH_WARN(_log,"convertDataType| Ignoring request not to scale; Scaling normalized float conversion to integral type to produce a valid range");
                    float2ucharScaled((unsigned char*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
                } else {
                    RH_DEBUG(_log,"convertDataType| Requested no scaling for non-normalized float conversion to integral type");
                    float2uchar((unsigned char*)&(dest[0]), (const float*)&(source[0]),size);
                }
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (float)) {
                std::copy(source, source+size, dest);
            }
            ///////////////////////////DOUBLE//////////////////////////////
            // Only supports double to float; all other floating-point conversions should be from float
            else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (double)) {
                for (size_t i = 0; i < (size_t) size; i++) {
                    dest[i] = OUT_TYPE(source[i]);
                }
            }
            else {
                RH_ERROR(_log,"convertDataType| Ignoring Request not to scale, Did not match expected types. Will try to scale the output to produce a valid range");
                RH_ERROR(_log,"convertDataType| Types In: "<<typeid (IN_TYPE).name()<< " Out: " <<typeid (OUT_TYPE).name());
                double tmpFloat;
                for (size_t i = 0; i < (size_t) size; i++) {
                    tmpFloat = (source[i] - sMin)* (dRange / sRange) + dMin;
                    dest[i] = OUT_TYPE(tmpFloat);
                }
            }
        } else { // scale == true
            //////////////////////////////CHAR////////////////////////////
            if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (signed char)) {
                char2ucharScaled((unsigned char*) &(dest[0]), (const char*) &(source[0]), size);
            } else if ((typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (signed char))) {
                char2shortScaled((short*) &(dest[0]), (const char*) &(source[0]), size, (short) sMin, (short) (dRange / sRange), (short) dMin);
            } else if ((typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (signed char))) {
                char2ushortScaled((unsigned short*) &(dest[0]), (const char*) &(source[0]), size, (short) sMin, (short) (dRange / sRange), (short) dMin);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (signed char)) {
                char2floatScaled((float*) &(dest[0]), (const char*) &(source[0]), size, sMin, dRange, sRange, dMin);
            }
            //////////////////////////UNSIGNED CHAR///////////////////////
            else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2charScaled((char*) &(dest[0]), (const unsigned char*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2shortScaled((short*) &(dest[0]), (unsigned char*) &(source[0]), (short) size, (short) sMin, (short) (dRange / sRange), (short) dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2ushortScaled((unsigned short*) &(dest[0]), (unsigned char*) &(source[0]), size, (short) sMin, (short) (dRange / sRange), (short) dMin);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (unsigned char)) {
                uchar2floatScaled((float*) &(dest[0]), (const unsigned char*) &(source[0]), size, sMin, dRange, sRange, dMin);
            }
            //////////////////////////////SHORT////////////////////////////
            else if (typeid (OUT_TYPE) == typeid (signed char) && typeid (IN_TYPE) == typeid (short)) {
                short2charScaled((char*) &(dest[0]), (const short*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (short)) {
                short2ucharScaled((unsigned char*) &(dest[0]), (const short*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (short)) {
                short2ushortScaled((unsigned short*) &(dest[0]), (const short*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (short)) {
                short2floatScaled((float*) &(dest[0]), (const short*) &(source[0]), size, sMin, dRange, sRange, dMin);
            }
            //////////////////////////UNSIGNED SHORT///////////////////////
            else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (unsigned short)) {
                ushort2shortScaled((short*) &(dest[0]), (const unsigned short*) &(source[0]), size);
            } else if (typeid (OUT_TYPE) == typeid (signed char) && typeid (IN_TYPE) == typeid (unsigned short)) {
                ushort2charScaled((char*) &(dest[0]), (const unsigned short*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (unsigned short)) {
                ushort2ucharScaled((unsigned char*) &(dest[0]), (const unsigned short*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (unsigned short)) {
                ushort2floatScaled((float*) &(dest[0]), (const unsigned short*) &(source[0]), size, sMin, dRange, sRange, dMin);
            }
            //////////////////////////////FLOAT////////////////////////////
            else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (float)) {
                float2shortScaled((short*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (float)) {
                float2ushortScaled((unsigned short*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (signed char) && typeid (IN_TYPE) == typeid (float)) {
                float2charScaled((char*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (float)) {
                float2ucharScaled((unsigned char*) &(dest[0]), (const float*) &(source[0]), size, sMin, dRange, sRange, dMin);
            } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (float)) {
                float2floatScaled((float*) &dest[0], (const float*) &source[0], size, sMin, dRange, sRange, dMin);
            }
            ///////////////////////////DOUBLE//////////////////////////////
            // Only supports double to float; all other floating-point conversions should be from float
            // - since this is no different than the default implemenation below, just fall through to that
            //else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (double)) {
            //    double tmpFloat;
            //    for (size_t i = 0; i < (size_t) size; i++) {
            //        tmpFloat = (source[i] - sMin)* (dRange / sRange) + dMin;
            //        dest[i] = OUT_TYPE(tmpFloat);
            //    }
            //}
            else {
                RH_DEBUG(_log,"convertDataType| Using generic/default scaled conversion");
                RH_DEBUG(_log,"convertDataType| Types In: "<<typeid (IN_TYPE).name()<< " Out: " <<typeid (OUT_TYPE).name());
                double tmpFloat;
                for (size_t i = 0; i < (size_t) size; i++) {
                    tmpFloat = (source[i] - sMin)* (dRange / sRange) + dMin;
                    dest[i] = OUT_TYPE(tmpFloat);
                }
            }
        }
    }

    // Bounds given by user
    template <typename IN_TYPE, typename OUT_TYPE> void convertDataTypeRange(IN_TYPE* source, OUT_TYPE* dest, double sMin, double sRange, double dMin, double dRange, size_t size = 10000, LOGGER _log=LOGGER()) {

        if (!_log) {
            _log = rh_logger::Logger::getLogger("DataConverter_dataTypeTransformOpt");
            RH_DEBUG(_log, "convertDataTypeRange method passed null logger; creating logger "<<_log->getName());
        } else {
            RH_DEBUG(_log, "convertDataTypeRange method passed valid logger "<<_log->getName());
        }

        //////////////////////////////CHAR////////////////////////////
        if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (char)) {
            char2ucharScaled((unsigned char*) &(dest[0]), (const char*) &(source[0]), size);
        } else if ((typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (char))) {
            char2shortScaled((short*) &(dest[0]), (const char*) &(source[0]), size, (short) sMin, (short) (dRange / sRange), (short) dMin);
        } else if ((typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (char))) {
            char2ushortScaled((unsigned short*) &(dest[0]), (const char*) &(source[0]), size, (short) sMin, (short) (dRange / sRange), (short) dMin);
        } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (char)) {
            char2floatScaled((float*) &(dest[0]), (const char*) &(source[0]), size, sMin, dRange, sRange, dMin);
        }
        //////////////////////////UNSIGNED CHAR///////////////////////
        else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (unsigned char)) {
            uchar2charScaled((char*) &(dest[0]), (const unsigned char*) &(source[0]), size);
        } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (unsigned char)) {
            uchar2shortScaled((short*) &(dest[0]), (const unsigned char*) &(source[0]), (short) size, (short) sMin, (short) (dRange / sRange), (short) dMin);
        } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (unsigned char)) {
            uchar2ushortScaled((unsigned short*) &(dest[0]), (const unsigned char*) &(source[0]), size, (short) sMin, (short) (dRange / sRange), (short) dMin);
        } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (unsigned char)) {
            uchar2floatScaled((float*) &(dest[0]), (const unsigned char*) &(source[0]), size, sMin, dRange, sRange, dMin);
        }
        //////////////////////////////SHORT////////////////////////////
        else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (short)) {
            short2charScaled((char*) &(dest[0]), (const short*) &(source[0]), size, sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (short)) {
            short2ucharScaled((unsigned char*) &(dest[0]), (const short*) &(source[0]), size, sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (unsigned short) && typeid (IN_TYPE) == typeid (short)) {
            short2ushortScaled((unsigned short*) &(dest[0]), (const short*) &(source[0]), size);
        } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (short)) {
            short2floatScaled((float*) &(dest[0]), (const short*) &(source[0]), size, sMin, dRange, sRange, dMin);
        }
        //////////////////////////UNSIGNED SHORT///////////////////////
        else if (typeid (OUT_TYPE) == typeid (char) && typeid (IN_TYPE) == typeid (unsigned short)) {
            ushort2charScaled((char*) &(dest[0]), (const unsigned short*) &(source[0]), size, sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (unsigned char) && typeid (IN_TYPE) == typeid (unsigned short)) {
            ushort2ucharScaled((unsigned char*) &(dest[0]), (const unsigned short*) &(source[0]), size, sMin, dRange, sRange, dMin);
        } else if (typeid (OUT_TYPE) == typeid (short) && typeid (IN_TYPE) == typeid (unsigned short)) {
            ushort2shortScaled((short*) &(dest[0]), (const unsigned short*) &(source[0]), size);
        } else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (unsigned short)) {
            ushort2floatScaled((float*) &(dest[0]), (const unsigned short*) &(source[0]), size, sMin, dRange, sRange, dMin);
        }
        //////////////////////////////FLOAT////////////////////////////
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
        ///////////////////////////DOUBLE//////////////////////////////
        // Only supports double to float; all other floating-point conversions should be from float
        // - since this is no different than the default implemenation below, just fall through to that
        //else if (typeid (OUT_TYPE) == typeid (float) && typeid (IN_TYPE) == typeid (double)) {
        //    double tmpFloat;
        //    for (size_t i = 0; i < (size_t) size; i++) {
        //        tmpFloat = (source[i] - sMin)* (dRange / sRange) + dMin;
        //        dest[i] = OUT_TYPE(tmpFloat);
        //    }
        //}
        else {
            RH_DEBUG(_log,"convertDataTypeRange| Using generic/default scaled conversion");
            RH_DEBUG(_log,"convertDataTypeRange| Types In: "<<typeid (IN_TYPE).name()<< " Out: " <<typeid (OUT_TYPE).name());
            double tmpFloat;
            for (size_t i = 0; i < (size_t) size; i++) {
                tmpFloat = (source[i] - sMin)* (dRange / sRange) + dMin;
                dest[i] = OUT_TYPE(tmpFloat);
            }
        }
    }
}


#endif    /* _DATATYPETRANSFORM_H */
