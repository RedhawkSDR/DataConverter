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

#ifndef DATACONVERTER_IMPL_H
#define DATACONVERTER_IMPL_H

#include "DataConverter_base.h"
#include <ossie/prop_helpers.h>
#include <vector>
#include "dataTypeTransformOpt.h"
#include "mathOptimizations.h"
#include <fftw3.h>
#include "R2C.h"

class OutDoublePortUsingFloats : public bulkio::OutDoublePort {
    
  public:
    OutDoublePortUsingFloats(std::string name) :
      bulkio::OutDoublePort(name) 
    {}
   
    void pushPacket(float* data, size_t size, BULKIO::PrecisionUTCTime& T, bool EOS, const std::string& streamId) {
        std::vector<double> doubleVector;
        for (size_t i = 0; i < size; i++) {
            doubleVector.push_back((double)data[i]);
        }
        bulkio::OutDoublePort::pushPacket(doubleVector, T, EOS, streamId);
    }
    
    void pushPacket(std::vector<float, std::allocator<float> > data, BULKIO::PrecisionUTCTime& T, bool EOS, const std::string& streamId) {
        std::vector<double> doubleVector(data.begin(), data.end());
        bulkio::OutDoublePort::pushPacket(doubleVector, T, EOS, streamId);
    }
};


#define IPP_NUMBER_THREADS 4
#define CORBA_XFR_SIZE 2096640
#define MY_FFTW_FLAGS (FFTW_MEASURE)
#define N 1
#define FFTW_NUMBER_THREADS 4
class DataConverter_i;

class DataConverter_i : public DataConverter_base {
    ENABLE_LOGGING
public:
    DataConverter_i(const char *uuid, const char *label);
    ~DataConverter_i();
    //template <class IN_PORT_TYPE> bool singleService(IN_PORT_TYPE *dataPortIn);
    //template <typename OUT_TYPE, typename IN_TYPE, typename IN_TYPE_ALLOC, class OUT> void pushDataService(std::vector<IN_TYPE, IN_TYPE_ALLOC> *data, OUT *output, bool EOS, BULKIO::PrecisionUTCTime& tt, std::string streamID, bool scaled);
    void transformPropertiesChanged(const transformProperties_struct* oldVal, const transformProperties_struct* newVal);
    void maxTransferSizeChanged(const CORBA::Long* oldVal, const CORBA::Long* newVal);
    void outputTypeChanged(const short* oldVal, const short* newVal);
    int serviceFunction();
    void constructor();

private:
    FILE* debugR2C;
    void createMemory(int bufferSize);
    void createFFTroute(int bufferSize);
    int calculate_memorySize(int inputAmount);
    void setupFFT();
    void configureSRI(BULKIO::StreamSRI *sri, bool incomingSRI=true);
    int fft_execute(float* data, int size, bool EOS);
    void updateTimeStamp();
    void moveTimeStamp(int shiftBack);
    void adjustTimeStamp(BULKIO::PrecisionUTCTime& timestamp, BULKIO::PrecisionUTCTime& localtimestamp);

    //FFT constants
    int _readIndex;
    int _writeIndex;
    int _samplesSinceLastTimestamp;
    bool first_overlap;
    int _loadedDataCount;
    int overlapAmount;
    int overlapRemovalPosition;
    float multiple;
    int fftSize;
    float mixerPhase;
    bool even_odd;
    int _outWriteIndex;
    bool firstRun;
    bool createMEM;
    bool createFFT;
    int _transferSize;
    char* outputB;
    char* conversionBuffer;
    //std::vector<char> outputB;
    int count;

    int remainder_v;
    //filter class
    void createFilter();
    R2C* filter;
    int numFilterTaps;

    //SRI and Time
    BULKIO::StreamSRI* currSRIPtr;
    BULKIO::PrecisionUTCTime _timestamp;
    double _FFTtimestampSec;
    double _FFTtimestampFractionalSec;

    //constants
    int fftType;
    int inputMode;
    int outputMode;
    double sampleRate;

    //locks
    boost::mutex property_lock;

    //fftwf memory and plans
    fftwf_plan r2c;
    fftwf_plan c2c_r2;
    fftwf_plan c2c_r;
    fftwf_plan c2c_f;

    //fftwf_complex* _in;
    //fftwf_complex* _in_r;
    //fftwf_complex* _out;
    fftwf_complex* complexTempBuffer;
    fftwf_complex* workingBuffer;
    fftwf_complex* tempBuffer;
    fftwf_complex* transferOut;
    float* fftRBuffer;
    fftwf_complex* fftBuffer;

    fftwf_complex* transformedBuffer;
    float* transformedRBuffer;
    float* floatBuffer;
    fftwf_complex* complexBuffer;


    fftwf_complex* r2cTempBuffer;
    fftwf_complex* upsampled;
    int dataAmount;
    
    OutDoublePortUsingFloats* _outDoublePort;

    template <typename T>
    bool isFloatingType(T* type=NULL) {
        if (typeid (T) == typeid(float) ||
            typeid (T) == typeid(double))
            return true;
        return false;
    }
    
    template <typename OUT_TYPE, typename IN_TYPE, typename IN_TYPE_ALLOC, typename OUT_PORT> 
    void pushDataService(std::vector<IN_TYPE, IN_TYPE_ALLOC> *data, OUT_PORT *output, bool EOS, BULKIO::PrecisionUTCTime& tt, std::string streamID, bool scaled) {
        
        float sRange = float(std::numeric_limits<IN_TYPE>::max()) - float(std::numeric_limits<IN_TYPE>::min());
        float sMin = float(std::numeric_limits<IN_TYPE>::min());
        if (normalize_floating_point.input) {
            sRange = 2;
            sMin = -1;
        } else {
            sRange = floatingPointRange.maximum - floatingPointRange.minimum;
            sMin = floatingPointRange.minimum;
        }
        float dRange = float(std::numeric_limits<OUT_TYPE>::max()) - float(std::numeric_limits<OUT_TYPE>::min());
        float dMin = (float) (std::numeric_limits<OUT_TYPE>::min());

        if (typeid (OUT_TYPE) == typeid (float) && normalize_floating_point.output) {
            dMin = -1.0;
            dRange = 2.0;
        }
        unsigned int sized = data->size();

        int memoryFootPrint = 0;
       
        if (output->isActive()) {
             count++;
            if (createMEM || firstRun) {
                memoryFootPrint = calculate_memorySize(sized);
                createMemory(memoryFootPrint);
                if (!createFFT)
                    firstRun = false;
            }

            if (isFloatingType<IN_TYPE>() && typeid (IN_TYPE) == typeid (OUT_TYPE) && fftType == 0 && (normalize_floating_point.input == normalize_floating_point.output)) {
                output->pushPacket(*((std::vector<OUT_TYPE>*)data), tt, EOS, streamID);
            } else if (typeid (IN_TYPE) == typeid (OUT_TYPE) && fftType == 0 && !isFloatingType<IN_TYPE>()) {
                output->pushPacket(*((std::vector<OUT_TYPE>*)data), tt, EOS, streamID);
            } else {
                if (fftType != 0) {
                    if ((createFFT || firstRun) && fftType > 0) {
                        createFFTroute(memoryFootPrint);
                        firstRun = false;
                    }
                    if (!isFloatingType<IN_TYPE>() && fftType > 0) {
                        dataTypeTransformOpt::convertDataType<IN_TYPE, float>((IN_TYPE*)&(*data)[0], (float*) &conversionBuffer[0], scaled, normalize_floating_point.input, normalize_floating_point.output, sized);
                        sized = fft_execute((float*) &conversionBuffer[0], sized, EOS);
                    } else if (typeid (IN_TYPE) == typeid (float) && fftType > 0) {
                        sized = fft_execute((float*) &(*data)[0], sized, EOS);
                    }
                }

                if (sized > (unsigned int) _transferSize) {
                    int dataBufferS = 0;
                    
                    dataBufferS = _transferSize;
                    int count = 0;
                    bool EOS_l = false;
                    for (int i = 0; i < (int) (sized / dataBufferS); ++i) {
                        count++;
                        if (sized - (count * dataBufferS) == 0 && EOS)
                            EOS_l = true;

                        if (fftType > 0 && (typeid (OUT_TYPE) == typeid (float))) {
                            if (normalize_floating_point.output) {
                                if (fftType == 1)
                                    float2floatScaled((float*) &outputB[0], (float*) &transformedRBuffer[i * dataBufferS], dataBufferS, sMin, dRange, sRange, dMin);
                                else
                                    float2floatScaled((float*) &outputB[0], (float*) &(*transformedBuffer)[i * dataBufferS], dataBufferS, sMin, dRange, sRange, dMin);
                                output->pushPacket((OUT_TYPE*) & outputB[0], dataBufferS, tt, EOS_l, streamID);
                            } else {
                                if (fftType == 1) {
                                    output->pushPacket((OUT_TYPE*)&((transformedRBuffer)[i * dataBufferS]), dataBufferS, tt, EOS_l, streamID);
                                } else {
                                    output->pushPacket((OUT_TYPE*)&((*transformedBuffer)[i * dataBufferS]), dataBufferS, tt, EOS_l, streamID);
                                }
                            }
                        } else if (fftType > 0 && !isFloatingType<IN_TYPE>()) {
                            if (fftType == 1) {
                                dataTypeTransformOpt::convertDataType<float, OUT_TYPE>((float*) &transformedRBuffer[i * dataBufferS], (OUT_TYPE*) & outputB[0], scaled, normalize_floating_point.input, false, dataBufferS);
                            } else {
                                dataTypeTransformOpt::convertDataType<float, OUT_TYPE>((float*) &((*transformedBuffer)[i * dataBufferS]), (OUT_TYPE*) & outputB[0], scaled, normalize_floating_point.input, false, dataBufferS);
                            }
                            output->pushPacket((OUT_TYPE*) & outputB[0], dataBufferS, tt, EOS_l, streamID);
                        } else {
                            if (isFloatingType<IN_TYPE>() && !normalize_floating_point.input) {
                                dataTypeTransformOpt::convertDataTypeRange<IN_TYPE, OUT_TYPE>((IN_TYPE*)&(*data)[i * dataBufferS], (OUT_TYPE*) & outputB[0], sMin, sRange, dMin, dRange, dataBufferS);
                            } else {
                                dataTypeTransformOpt::convertDataType<IN_TYPE, OUT_TYPE>((IN_TYPE*)&(*data)[i * dataBufferS], (OUT_TYPE*) & outputB[0], scaled, normalize_floating_point.input, normalize_floating_point.output, dataBufferS);
                            }
                            output->pushPacket((OUT_TYPE*) & outputB[0], dataBufferS, tt, EOS_l, streamID);
                        }
                        tt.tfsec = tt.tfsec + ((1.0 / sampleRate) * i * dataBufferS);
                        if (tt.tfsec >= 1) {
                            tt.twsec++;
                            --tt.tfsec;
                        }
                    }
                    if (sized - (count * dataBufferS) != 0) {
                        int leftover = (sized - (count * dataBufferS));
                        if (fftType > 0 && (typeid (OUT_TYPE) == typeid (float))) {
                            if (normalize_floating_point.output) {
                                if (fftType == 1)
                                    float2floatScaled((float*) &(outputB[0]), (const float*) &transformedRBuffer[sized - leftover], leftover, sMin, dRange, sRange, dMin);
                                else
                                    float2floatScaled((float*) &(outputB[0]), (const float*) &((*transformedBuffer)[sized - leftover]), leftover, sMin, dRange, sRange, dMin);
                                output->pushPacket((OUT_TYPE*) & outputB[0], leftover, tt, EOS, streamID);
                            } else {
                                output->pushPacket((OUT_TYPE*)&((*transformedBuffer)[sized - leftover]), leftover, tt, EOS, streamID);
                            }
                        } else if (fftType > 0 && typeid (OUT_TYPE) != typeid (float)) {
                            if (fftType == 1) {
                                dataTypeTransformOpt::convertDataType<float, OUT_TYPE>((float*) &transformedRBuffer[sized - leftover], (OUT_TYPE*) & outputB[0], scaled, normalize_floating_point.input, normalize_floating_point.output, leftover);
                            } else {
                                dataTypeTransformOpt::convertDataType<float, OUT_TYPE>((float*) &(*transformedBuffer)[sized - leftover], (OUT_TYPE*) & outputB[0], scaled, normalize_floating_point.input, normalize_floating_point.output, leftover);
                            }
                            output->pushPacket((OUT_TYPE*) & outputB[0], leftover, tt, EOS, streamID);
                        } else {
                            if (isFloatingType<IN_TYPE>() && !normalize_floating_point.input) {
                                dataTypeTransformOpt::convertDataTypeRange<IN_TYPE, OUT_TYPE>((IN_TYPE*)&(*data)[sized - leftover], (OUT_TYPE*) & outputB[0], sMin, sRange, dMin, dRange, leftover);
                            } else {
                                dataTypeTransformOpt::convertDataType<IN_TYPE, OUT_TYPE>((IN_TYPE*)&(*data)[sized - leftover], (OUT_TYPE*) & outputB[0], scaled, normalize_floating_point.input, normalize_floating_point.output, leftover);
                            }
                            output->pushPacket((OUT_TYPE*) & outputB[0], leftover, tt, EOS, streamID);
                        }
                    }
                } else {
                    if (fftType > 0 && (typeid (OUT_TYPE) == typeid (float))) {
                        if (normalize_floating_point.output) {
                            if (fftType == 1)
                                float2floatScaled((float*) &outputB[0], (const float*) &transformedRBuffer[0], sized, sMin, dRange, sRange, dMin);
                            else
                                float2floatScaled((float*) &outputB[0], (const float*) &(*transformedBuffer)[0], sized, sMin, dRange, sRange, dMin);
                            output->pushPacket((OUT_TYPE*) & outputB[0], sized, tt, EOS, streamID);
                        } else {
                            output->pushPacket((OUT_TYPE*)&(*transformedBuffer)[0], sized, tt, EOS, streamID);
                        }
                    } else if (fftType > 0 && typeid (OUT_TYPE) != typeid (float)) {
                        if (fftType == 1) {
                            dataTypeTransformOpt::convertDataType<float, OUT_TYPE>((float*) &(transformedRBuffer[0]), (OUT_TYPE*) & outputB[0], scaled, true, true, sized);
                            output->pushPacket((OUT_TYPE*)&(outputB[0]), sized, tt, EOS, streamID);
                        } else {
                            dataTypeTransformOpt::convertDataType<float, OUT_TYPE>((float*) &(transformedBuffer[0]), (OUT_TYPE*) & outputB[0], scaled, true, true, sized);
                            output->pushPacket((OUT_TYPE*)&(outputB[0]), sized, tt, EOS, streamID);
                        }
                    } else {
                        if (isFloatingType<IN_TYPE>() && !normalize_floating_point.input) {
                            dataTypeTransformOpt::convertDataTypeRange<IN_TYPE, OUT_TYPE>((IN_TYPE*)&(*data)[0], (OUT_TYPE*)&(outputB[0]), sMin, sRange, dMin, dRange, sized);
                        } else {
                            dataTypeTransformOpt::convertDataType<IN_TYPE, OUT_TYPE>((IN_TYPE*)&(*data)[0], (OUT_TYPE*)&(outputB[0]), scaled, normalize_floating_point.input, normalize_floating_point.output, sized);
                        }
                        output->pushPacket((OUT_TYPE*)&(outputB[0]), sized, tt, EOS, streamID);
                    }
                }
            }
      }
    };

    template <class IN_PORT_TYPE> bool singleService(IN_PORT_TYPE *dataPortIn) {
        boost::mutex::scoped_lock lock(property_lock);

        typename IN_PORT_TYPE::dataTransfer *packet = dataPortIn->getPacket(0);
        if (packet == NULL)
            return false;    
        if (packet->inputQueueFlushed)
            std::cout << "INPUT QUEUE HAS FLUSHED!!!!!!" << std::endl;
        // Reconfigure if SRI Changed
        if (packet->sriChanged || !currSRIPtr) {
            configureSRI(&(packet->SRI));
        }
        if (packet->T.tcstatus == BULKIO::TCS_VALID && fftType != 0) {
            adjustTimeStamp(packet->T, _timestamp);
            _samplesSinceLastTimestamp = 0;
        } else if (packet->T.tcstatus == BULKIO::TCS_VALID) {
            _timestamp = packet->T;
        } else if (packet->T.tcstatus != BULKIO::TCS_VALID) {
            LOG_WARN(DataConverter_i, "Invalid, T.tcstatus Sending invalid timecode anyway. tcstatus:" << packet->T.tcstatus);
            _timestamp = packet->T;
        }

        //typename IN_PORT_TYPE::dataTransfer::DataBufferType dataBuffer = packet->dataBuffer;
        if (typeid(typename IN_PORT_TYPE::dataTransfer::DataBufferType::value_type) == typeid(double)) {           
            // Convert incoming doubles to floats
            std::vector<float> dataBuffer(packet->dataBuffer.begin(), packet->dataBuffer.end());
            
            pushDataService<char>(&dataBuffer, dataChar_out, packet->EOS, _timestamp, packet->streamID, scaleOutput.charPort);
            pushDataService<unsigned char>(&dataBuffer, dataOctet_out, packet->EOS, _timestamp, packet->streamID, scaleOutput.octetPort);//,normalize_floating_point.normalized_input);
            pushDataService<short>(&dataBuffer, dataShort_out, packet->EOS, _timestamp, packet->streamID, scaleOutput.shortPort); //,normalize_floating_point.normalized_input);
            pushDataService<unsigned short>(&dataBuffer, dataUshort_out, packet->EOS, _timestamp, packet->streamID, scaleOutput.ushortPort);//,normalize_floating_point.normalized_input);
            pushDataService<float>(&dataBuffer, dataFloat_out, packet->EOS, _timestamp, packet->streamID,normalize_floating_point.output);//,normalize_floating_point.normalized_output);
            pushDataService<float>(&(packet->dataBuffer), _outDoublePort, packet->EOS, _timestamp, packet->streamID,normalize_floating_point.output);//,normalize_floating_point.normalized_output);
        } else {
            typename IN_PORT_TYPE::dataTransfer::DataBufferType dataBuffer = packet->dataBuffer;
            pushDataService<char>(&dataBuffer, dataChar_out, packet->EOS, _timestamp, packet->streamID, scaleOutput.charPort);
            pushDataService<unsigned char>(&dataBuffer, dataOctet_out, packet->EOS, _timestamp, packet->streamID, scaleOutput.octetPort);//,normalize_floating_point.normalized_input);
            pushDataService<short>(&dataBuffer, dataShort_out, packet->EOS, _timestamp, packet->streamID, scaleOutput.shortPort); //,normalize_floating_point.normalized_input);
            pushDataService<unsigned short>(&dataBuffer, dataUshort_out, packet->EOS, _timestamp, packet->streamID, scaleOutput.ushortPort);//,normalize_floating_point.normalized_input);
            pushDataService<float>(&dataBuffer, dataFloat_out, packet->EOS, _timestamp, packet->streamID,normalize_floating_point.output);//,normalize_floating_point.normalized_output);
            pushDataService<float>(&dataBuffer, _outDoublePort, packet->EOS, _timestamp, packet->streamID,normalize_floating_point.output);//,normalize_floating_point.normalized_output);
        }
        
        /* delete the dataTransfer object */
        delete packet;
        return true;
    };


};

#endif
