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
#include "dataTypeTransformOpt.h"
#include "mathOptimizations.h"
#include <fftw3.h>
#include "R2C.h"
#include <boost/thread/mutex.hpp>
#include "FftwThreadCoordinator.h"

#define MY_FFTW_FLAGS (FFTW_MEASURE)
//#define FFTW_NUMBER_THREADS 4

class DataConverter_i : public DataConverter_base {
    ENABLE_LOGGING
public:
    DataConverter_i(const char *uuid, const char *label);
    ~DataConverter_i();
    void transformPropertiesChanged(const transformProperties_struct &oldVal, const transformProperties_struct &newVal);
    void maxTransferSizeChanged(CORBA::Long oldVal, CORBA::Long newVal);
    void outputTypeChanged(short oldVal, short newVal);
    int serviceFunction();
    void constructor();

private:
    //FILE* debugR2C;
    void createMemory(int bufferSize);
    void createFFTroute(int bufferSize);
    int calculate_memorySize(int inputAmount);
    void setupFFT();
    void configureSRI(const BULKIO::StreamSRI& sriIn, bool incomingSRI=true);
    int fft_execute(float* data, int size, bool EOS);
    void moveTimeStamp(int shiftBack);
    void adjustTimeStamp(const BULKIO::PrecisionUTCTime& timestamp, BULKIO::PrecisionUTCTime& localtimestamp);
    void flush();

    //FFT constants
    int _readIndex;
    int _writeIndex;
    bool first_overlap;
    int _loadedDataCount;
    int overlapAmount;
    int overlapRemovalPosition;
    float multiple;
    int fftSize;
    float mixerPhase;
    bool even_odd;
    int _outWriteIndex;
    bool createMEM;
    bool createFFT;
    unsigned int maxSamplesIn;
    int _transferSize;
    char* outputBuffer;
    char* conversionBuffer;
    int count;

    int remainder_v;
    //filter class
    void createFilter();
    R2C* filter;
    int numFilterTaps;

    //SRI and Time
    BULKIO::StreamSRI* currSRIPtr;
    BULKIO::PrecisionUTCTime _timestamp;

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

    fftwf_complex* complexTempBuffer;
    fftwf_complex* workingBuffer;
    fftwf_complex* tempBuffer;
    fftwf_complex* transferOut;
    float* fftRBuffer;
    fftwf_complex* fftBuffer;
    fftwf_complex* transformedBuffer;
    float* transformedRBuffer;
    fftwf_complex* r2cTempBuffer;
    fftwf_complex* upsampled;
    int dataAmount;

    template <typename T>
    bool isFloatingType(T* type=NULL) {
        if (typeid (T) == typeid(float) ||
                typeid (T) == typeid(double))
            return true;
        return false;
    }

    template <typename OUT_TYPE, typename IN_TYPE, typename OUT_PORT>
    void pushDataService(const redhawk::shared_buffer<IN_TYPE> data, OUT_PORT *output, bool EOS, BULKIO::PrecisionUTCTime& tt, std::string streamID, bool scaled) {

        LOG_DEBUG(DataConverter_i,"pushDataService|Types: Port: "<<typeid (OUT_PORT).name() << " In: "<<typeid (IN_TYPE).name()<< " Out: " <<typeid (OUT_TYPE).name());
        if (!output->isActive()) return;

        double sRange = double(std::numeric_limits<IN_TYPE>::max()) - double(std::numeric_limits<IN_TYPE>::min());
        double sMin = double(std::numeric_limits<IN_TYPE>::min());
        double dRange = double(std::numeric_limits<OUT_TYPE>::max()) - double(std::numeric_limits<OUT_TYPE>::min());
        double dMin = (double) (std::numeric_limits<OUT_TYPE>::min());

        if (isFloatingType<IN_TYPE>()) {
            if (normalize_floating_point.input) {
                sRange = 2.0;
                sMin = -1.0;
            } else {
                sRange = floatingPointRange.maximum - floatingPointRange.minimum;
                sMin = floatingPointRange.minimum;
            }
        }
        if (isFloatingType<OUT_TYPE>()) {
            if (normalize_floating_point.output) {
                dRange = 2.0;
                dMin = -1.0;
            } else {
                dRange = sRange;
                dMin = sMin;
            }
        }
        LOG_DEBUG(DataConverter_i,"pushDataService| sMin="<<sMin<<" sRange="<<sRange<<" dMin="<<dMin<<" dRange="<<dRange);

        unsigned int totalIn = data.size();
        LOG_DEBUG(DataConverter_i,"pushDataService|Input size="<<totalIn);
        int memoryFootPrint = 0;

        typename OUT_PORT::StreamType stream = output->getStream(streamID);
        LOG_TRACE(DataConverter_i,"pushDataService|before: inputMode="<<inputMode<<" outputMode="<<outputMode<<" outputType="<<outputType<<" fftType="<<fftType<<" output mode="<<stream.complex());
        count++;
        LOG_TRACE(DataConverter_i,"pushDataService|beforeMEM createMEM="<<createMEM<<" createFFT="<<createFFT);
        if (createMEM || (createFFT && fftType > 0) || totalIn > maxSamplesIn) {
            LOG_TRACE(DataConverter_i,"pushDataService|enterMEM createMEM="<<createMEM<<" createFFT="<<createFFT);
            maxSamplesIn = totalIn;
            memoryFootPrint = calculate_memorySize(totalIn);
            createMemory(memoryFootPrint);
            LOG_TRACE(DataConverter_i,"pushDataService|exitMEM createMEM="<<createMEM<<" createFFT="<<createFFT);
        }

        if (isFloatingType<IN_TYPE>() && typeid (IN_TYPE) == typeid (OUT_TYPE)
                && fftType == 0 && (normalize_floating_point.input || !normalize_floating_point.output)) {
            LOG_TRACE(DataConverter_i,"pushDataService|no conversion needed: same floating-point type, no change in mode, no floating point normalization");
            stream.write(redhawk::shared_buffer<OUT_TYPE>::recast(data),tt); if (EOS) stream.close();
        } else if (typeid (IN_TYPE) == typeid (OUT_TYPE) && fftType == 0 && !isFloatingType<IN_TYPE>()) {
            LOG_TRACE(DataConverter_i,"pushDataService|no conversion needed: same type, no change in mode");
            stream.write(redhawk::shared_buffer<OUT_TYPE>::recast(data),tt); if (EOS) stream.close();
        } else {
            if (fftType > 0) {
                LOG_TRACE(DataConverter_i,"pushDataService|fftmem (before) createMEM="<<createMEM<<" createFFT="<<createFFT);
                if (createFFT) {
                    LOG_TRACE(DataConverter_i,"pushDataService|fftmem (enter) createMEM="<<createMEM<<" createFFT="<<createFFT);
                    createFFTroute(memoryFootPrint);
                    LOG_TRACE(DataConverter_i,"pushDataService|fftmem (exit) createMEM="<<createMEM<<" createFFT="<<createFFT);
                }
                if (!isFloatingType<IN_TYPE>()) {
                    LOG_TRACE(DataConverter_i,"pushDataService|convert integral type to float and execute fft: len="<<totalIn<<" EOS="<<EOS);
                    dataTypeTransformOpt::convertDataType<IN_TYPE, float>((IN_TYPE*)&data[0], (float*) &conversionBuffer[0], scaled, false, scaled, totalIn);
                    totalIn = fft_execute((float*) &conversionBuffer[0], totalIn, EOS);
                } else if (typeid (IN_TYPE) == typeid (float)) {
                    LOG_TRACE(DataConverter_i,"pushDataService|execute fft on float input: len="<<totalIn<<" EOS="<<EOS);
                    if (scaled && !normalize_floating_point.input) {
                        float2floatScaled((float*) &conversionBuffer[0], (float*) &data[0], totalIn, sMin, 2, sRange, -1);
                        totalIn = fft_execute((float*) &conversionBuffer[0], totalIn, EOS);
                    } else {
                        totalIn = fft_execute((float*) &data[0], totalIn, EOS);
                    }
                } else if (typeid (IN_TYPE) == typeid (double)) {
                    LOG_TRACE(DataConverter_i,"pushDataService|convert double to float and execute fft: len="<<totalIn<<" EOS="<<EOS);
                    double floatMin = double(std::numeric_limits<float>::max())*-1;
                    double floatRange = double(std::numeric_limits<float>::max())*2;
                    if (scaled && !normalize_floating_point.input) {
                        dataTypeTransformOpt::convertDataTypeRange<IN_TYPE, float>((IN_TYPE*)&data[0], (float*) &conversionBuffer[0], sMin, sRange, -1, 2, totalIn);
                    } else if (sMin < floatMin || (sMin+sRange) > (floatMin+floatRange)) {
                        // if input is not within float range, scale such that it does fit when converting to float, then execute fft
                        // additional scaling or normalization will be done later if needed
                        LOG_WARN(DataConverter_i,"pushDataService|scaling double input to fit in float range");
                        LOG_INFO(DataConverter_i,"pushDataService|scaling vals: sMin="<<sMin<<" sRange="<<sRange<<" floatMin="<<floatMin<<" floatRange="<<floatRange);
                        dataTypeTransformOpt::convertDataTypeRange<IN_TYPE, float>((IN_TYPE*)&data[0], (float*) &conversionBuffer[0], sMin, sRange, floatMin, floatRange, totalIn);
                    } else {
                        dataTypeTransformOpt::convertDataType<IN_TYPE, float>((IN_TYPE*)&data[0], (float*) &conversionBuffer[0], false, normalize_floating_point.input, false, totalIn);
                    }
                    totalIn = fft_execute((float*) &conversionBuffer[0], totalIn, EOS);
                }
            }

            size_t local_transferSize = _transferSize; // store copy so it doesn't change during execution
            size_t totalSent = 0;
            size_t curBufferSize = 0;

            LOG_TRACE(DataConverter_i,"pushDataService|before outputloop - totalSent="<<totalSent<<" totalIn="<<totalIn<<" local_transferSize="<<local_transferSize);
            while(totalSent < totalIn) {
                LOG_TRACE(DataConverter_i,"pushDataService|outputloop - totalSent="<<totalSent<<" totalIn="<<totalIn<<" local_transferSize="<<local_transferSize);
                curBufferSize = std::min(totalIn-totalSent, local_transferSize);
                LOG_TRACE(DataConverter_i,"pushDataService|outputloop - curBufferSize="<<curBufferSize);
                if (fftType > 0 && (typeid (OUT_TYPE) == typeid (float))) { // all FFT conversions to float
                    LOG_TRACE(DataConverter_i,"pushDataService|outputloop did FFT and output type is float");
                    if (fftType == 1)
                        stream.write((OUT_TYPE*)&((transformedRBuffer)[totalSent]), curBufferSize, tt);
                    else
                        stream.write((OUT_TYPE*)&((*transformedBuffer)[totalSent]), curBufferSize, tt);
                } else if (fftType > 0 && (typeid (OUT_TYPE) == typeid (double))) { // all FFT conversions to double
                    if (fftType == 1)
                        float2double((double*) &outputBuffer[0], (float*) &transformedRBuffer[totalSent], curBufferSize);
                    else
                        float2double((double*) &outputBuffer[0], (float*) &(*transformedBuffer)[totalSent], curBufferSize);
                    stream.write((OUT_TYPE*) & outputBuffer[0], curBufferSize, tt);
                } else if (fftType > 0) {// && !isFloatingType<OUT_TYPE>()) { // all FFT conversions to non-floating-point
                    LOG_TRACE(DataConverter_i,"pushDataService|FFT conversion to non-floating-point");
                    if (fftType == 1) {
                        dataTypeTransformOpt::convertDataType<float, OUT_TYPE>((float*) &transformedRBuffer[totalSent], (OUT_TYPE*) & outputBuffer[0], scaled, scaled, false, curBufferSize);
                    } else {
                        LOG_TRACE(DataConverter_i,"pushDataService|R2C conversion to non-float");
                        dataTypeTransformOpt::convertDataType<float, OUT_TYPE>((float*) &((*transformedBuffer)[totalSent]), (OUT_TYPE*) & outputBuffer[0], scaled, scaled, false, curBufferSize);
                    }
                    LOG_TRACE(DataConverter_i,"pushDataService|FFT to non-float - writing out curBufferSize="<<curBufferSize);
                    stream.write((OUT_TYPE*) & outputBuffer[0], curBufferSize, tt);
                    LOG_TRACE(DataConverter_i,"pushDataService|FFT to non-float - DONE writing out curBufferSize="<<curBufferSize);
                } else if (typeid (IN_TYPE) == typeid (double)) { // all non-FFT conversions from double
                    if (typeid (OUT_TYPE) == typeid (double)) { // double to double normalization
                        double2doubleScaled((double*) &outputBuffer[0], (double*) &data[totalSent], curBufferSize, sMin, dRange, sRange, dMin);
                    } else {
                        // normalize float output if scaling anyway
                        if (!normalize_floating_point.input && scaled) { // double to normalized float
                            dataTypeTransformOpt::convertDataTypeRange<IN_TYPE, float>((IN_TYPE*)&data[totalSent], (float*) &conversionBuffer[0], sMin, sRange, -1, 2, curBufferSize);
                        } else { // double to float (either not normalized, or input already normalized)
                            dataTypeTransformOpt::convertDataType<IN_TYPE, float>((IN_TYPE*)&data[totalSent], (float*) &conversionBuffer[0], scaled, normalize_floating_point.input, scaled, curBufferSize);
                        }
                        // input below has been normalized if scaling
                        dataTypeTransformOpt::convertDataType<float, OUT_TYPE>((float*) &conversionBuffer[0], (OUT_TYPE*) & outputBuffer[0], scaled, normalize_floating_point.input||scaled, normalize_floating_point.output, curBufferSize);
                    }
                    stream.write((OUT_TYPE*) & outputBuffer[0], curBufferSize, tt);
                } else if (typeid (OUT_TYPE) == typeid (double)) { // non-FFT conversions not from double, but to double
                    if (isFloatingType<IN_TYPE>() && !normalize_floating_point.input && scaled) { // float to normalized double
                        float2doubleScaled((double*) &outputBuffer[0], (float*)&data[totalSent], curBufferSize, sMin, dRange, sRange, dMin);
                    } else {
                        dataTypeTransformOpt::convertDataType<IN_TYPE, float>((IN_TYPE*)&data[totalSent], (float*) & conversionBuffer[0], scaled, normalize_floating_point.input, normalize_floating_point.output, curBufferSize);
                        float2double((double*) &outputBuffer[0], (float*) &conversionBuffer[totalSent], curBufferSize);
                    }
                    stream.write((OUT_TYPE*) & outputBuffer[0], curBufferSize, tt);
                } else { // all non-FFT conversions not from or to double
                    if (isFloatingType<IN_TYPE>() && !normalize_floating_point.input && scaled) {
                        dataTypeTransformOpt::convertDataTypeRange<IN_TYPE, OUT_TYPE>((IN_TYPE*)&data[totalSent], (OUT_TYPE*) & outputBuffer[0], sMin, sRange, dMin, dRange, curBufferSize);
                    } else {
                        dataTypeTransformOpt::convertDataType<IN_TYPE, OUT_TYPE>((IN_TYPE*)&data[totalSent], (OUT_TYPE*) & outputBuffer[0], scaled, normalize_floating_point.input, normalize_floating_point.output, curBufferSize);
                    }
                    stream.write((OUT_TYPE*) & outputBuffer[0], curBufferSize, tt);
                }
                totalSent+=curBufferSize;
                tt.tfsec = tt.tfsec + ((1.0 / sampleRate) * curBufferSize);
                bulkio::time::utils::normalize(tt);
                LOG_DEBUG(DataConverter_i,"pushDataService|loop DONE; wrote curBufferSize="<<curBufferSize<<" more for total of totalSent="<<totalSent);
            }
            if (EOS) stream.close();
        }
    }

    template <class IN_PORT_TYPE> bool singleService(IN_PORT_TYPE *dataPortIn) {
        boost::mutex::scoped_lock lock(property_lock);

        typename IN_PORT_TYPE::StreamType stream = dataPortIn->getCurrentStream(0);
        if (!stream) {
            return false; // NOOP
        }

        typename IN_PORT_TYPE::StreamType::DataBlockType block = stream.read();
        if (!block) {
            if (stream.eos()) {
                LOG_DEBUG(DataConverter_i,"Received empty EOS block for stream "<<stream.streamID());
            } else {
                LOG_DEBUG(DataConverter_i,"Received empty non-EOS block for stream "<<stream.streamID());
                return false; // NOOP
            }
        } else {

            if (block.inputQueueFlushed()) {
                LOG_WARN(DataConverter_i, "Input queue flushed.  Flushing internal buffers.");
                flush();
            }

            if (block.sriChanged() || !currSRIPtr) {
                LOG_DEBUG(DataConverter_i,"Configuring stream "<<stream.streamID()<<" with updated SRI");
                configureSRI(block.sri());
                // currSRIPtr now is a pointer to resulting sri
            }

            if (block.getStartTime().tcstatus == BULKIO::TCS_VALID && fftType != 0) {
                adjustTimeStamp(block.getStartTime(), _timestamp);
            } else if (block.getStartTime().tcstatus == BULKIO::TCS_VALID) {
                _timestamp = block.getStartTime();
            } else {// if (block.getStartTime().tcstatus != BULKIO::TCS_VALID) {
                LOG_WARN(DataConverter_i, "Invalid, T.tcstatus Sending invalid timecode anyway. tcstatus:" << block.getStartTime().tcstatus);
                _timestamp = block.getStartTime();
            }
        }

        redhawk::shared_buffer<typename IN_PORT_TYPE::dataTransfer::DataBufferType::value_type> dataBuffer =
                !block ?
                        redhawk::shared_buffer<typename IN_PORT_TYPE::dataTransfer::DataBufferType::value_type>() :
                        block.buffer();
        pushDataService<signed char>(dataBuffer, dataChar_out, stream.eos(), _timestamp, stream.streamID(), scaleOutput.charPort);
        pushDataService<unsigned char>(dataBuffer, dataOctet_out, stream.eos(), _timestamp, stream.streamID(), scaleOutput.octetPort);//,normalize_floating_point.normalized_input);
        pushDataService<short>(dataBuffer, dataShort_out, stream.eos(), _timestamp, stream.streamID(), scaleOutput.shortPort); //,normalize_floating_point.normalized_input);
        pushDataService<unsigned short>(dataBuffer, dataUshort_out, stream.eos(), _timestamp, stream.streamID(), scaleOutput.ushortPort);//,normalize_floating_point.normalized_input);
        pushDataService<float>(dataBuffer, dataFloat_out, stream.eos(), _timestamp, stream.streamID(),normalize_floating_point.output);//,normalize_floating_point.normalized_output);
        pushDataService<double>(dataBuffer, dataDouble_out, stream.eos(), _timestamp, stream.streamID(),normalize_floating_point.output);//,normalize_floating_point.normalized_output);

        return true; // NORMAL
    }

};



#endif
