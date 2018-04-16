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

namespace {
    static boost::mutex fftw_plan_mutex;
}

#define IPP_NUMBER_THREADS 4
#define MY_FFTW_FLAGS (FFTW_MEASURE)
#define N 1
#define FFTW_NUMBER_THREADS 4
class DataConverter_i;

class DataConverter_i : public DataConverter_base {
    ENABLE_LOGGING
public:
    DataConverter_i(const char *uuid, const char *label);
    ~DataConverter_i();
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
    void configureSRI(const BULKIO::StreamSRI& sriIn, bool incomingSRI=true);
    int fft_execute(float* data, int size, bool EOS);
    //void updateTimeStamp();
    void moveTimeStamp(int shiftBack);
    void adjustTimeStamp(const BULKIO::PrecisionUTCTime& timestamp, BULKIO::PrecisionUTCTime& localtimestamp);

    //FFT constants
    int _readIndex;
    int _writeIndex;
    //int _samplesSinceLastTimestamp;
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
    //double _FFTtimestampSec;
    //double _FFTtimestampFractionalSec;

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
    float* floatBuffer;
    fftwf_complex* complexBuffer;
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

        unsigned int sized = data.size(); // TODO FIXME - this size could change from loop to loop without adjusting memory footprint... segfault waiting to happen
        int memoryFootPrint = 0;

        typename OUT_PORT::StreamType stream = output->getStream(streamID);
        LOG_DEBUG(DataConverter_i,"pushDataService|before: inputMode="<<inputMode);
        LOG_DEBUG(DataConverter_i,"pushDataService|before: outputMode="<<outputMode);
        LOG_DEBUG(DataConverter_i,"pushDataService|before: outputType="<<outputType);
        LOG_DEBUG(DataConverter_i,"pushDataService|before: fftType="<<fftType);
        LOG_DEBUG(DataConverter_i,"pushDataService|Input size="<<sized);
        LOG_DEBUG(DataConverter_i,"pushDataService|Output mode="<<stream.complex());
        count++;
        LOG_DEBUG(DataConverter_i,"pushDataService|beforeMEM createMEM="<<createMEM<<" createFFT="<<createFFT<<" firstRun="<<firstRun);
        if (createMEM || firstRun) {
            LOG_DEBUG(DataConverter_i,"pushDataService|enterMEM createMEM="<<createMEM<<" createFFT="<<createFFT<<" firstRun="<<firstRun);
            memoryFootPrint = calculate_memorySize(sized);
            createMemory(memoryFootPrint);
            if (!createFFT)
                firstRun = false;
            LOG_DEBUG(DataConverter_i,"pushDataService|exitMEM createMEM="<<createMEM<<" createFFT="<<createFFT<<" firstRun="<<firstRun);
        }

        if (isFloatingType<IN_TYPE>() && typeid (IN_TYPE) == typeid (OUT_TYPE)
                && fftType == 0 && (normalize_floating_point.input || !normalize_floating_point.output)) {
            // no conversion needed - same floating-point type, no change in mode, no floating point normalization
            stream.write(redhawk::shared_buffer<OUT_TYPE>::recast(data),tt); if (EOS) stream.close();
        } else if (typeid (IN_TYPE) == typeid (OUT_TYPE) && fftType == 0 && !isFloatingType<IN_TYPE>()) {
            // no conversion needed - same type, not change in mode
            stream.write(redhawk::shared_buffer<OUT_TYPE>::recast(data),tt); if (EOS) stream.close();
        } else {
            if (fftType > 0) {
                LOG_DEBUG(DataConverter_i,"pushDataService|beforeFFT createMEM="<<createMEM<<" createFFT="<<createFFT<<" firstRun="<<firstRun);
                if ((createFFT || firstRun)) {
                    LOG_DEBUG(DataConverter_i,"pushDataService|enterFFT createMEM="<<createMEM<<" createFFT="<<createFFT<<" firstRun="<<firstRun);
                    createFFTroute(memoryFootPrint);
                    firstRun = false;
                    LOG_DEBUG(DataConverter_i,"pushDataService|exitFFT createMEM="<<createMEM<<" createFFT="<<createFFT<<" firstRun="<<firstRun);
                }
                if (!isFloatingType<IN_TYPE>()) {
                    dataTypeTransformOpt::convertDataType<IN_TYPE, float>((IN_TYPE*)&data[0], (float*) &conversionBuffer[0], scaled, normalize_floating_point.input, normalize_floating_point.output, sized);
                    sized = fft_execute((float*) &conversionBuffer[0], sized, EOS);
                } else if (typeid (IN_TYPE) == typeid (float)) {
                    sized = fft_execute((float*) &data[0], sized, EOS);
                } else if (typeid (IN_TYPE) == typeid (double)) {
                    LOG_DEBUG(DataConverter_i,"pushDataService|doubleIn - converting to float, sized="<<sized);
                    dataTypeTransformOpt::convertDataType<IN_TYPE, float>((IN_TYPE*)&data[0], (float*) &conversionBuffer[0], false, normalize_floating_point.input, normalize_floating_point.output, sized);
                    LOG_DEBUG(DataConverter_i,"pushDataService|doubleIn - executing fft, sized="<<sized);
                    sized = fft_execute((float*) &conversionBuffer[0], sized, EOS);
                    LOG_DEBUG(DataConverter_i,"pushDataService|doubleIn - DONE executing fft, sized="<<sized);
                }
            }

            size_t totalIn = sized; // TODO
            size_t local_transferSize = _transferSize; // store copy so it doesn't change during execution
            size_t totalSent = 0;
            size_t curBufferSize = 0;

            LOG_DEBUG(DataConverter_i,"pushDataService|before loop - totalSent="<<totalSent<<" totalIn="<<totalIn<<" local_transferSize="<<local_transferSize);
            while(totalSent < totalIn) {
                LOG_DEBUG(DataConverter_i,"pushDataService|loop - totalSent="<<totalSent<<" totalIn="<<totalIn<<" local_transferSize="<<local_transferSize);
                curBufferSize = std::min(totalIn-totalSent, local_transferSize);
                LOG_DEBUG(DataConverter_i,"pushDataService|loop - curBufferSize="<<curBufferSize);
                if (fftType > 0 && (typeid (OUT_TYPE) == typeid (float))) { // all FFT conversions to float
                    if (normalize_floating_point.output) {
                        if (fftType == 1)
                            float2floatScaled((float*) &outputBuffer[0], (float*) &transformedRBuffer[totalSent], curBufferSize, sMin, dRange, sRange, dMin);
                        else
                            float2floatScaled((float*) &outputBuffer[0], (float*) &(*transformedBuffer)[totalSent], curBufferSize, sMin, dRange, sRange, dMin);
                        stream.write((OUT_TYPE*) & outputBuffer[0], curBufferSize, tt);
                    } else {
                        if (fftType == 1)
                            stream.write((OUT_TYPE*)&((transformedRBuffer)[totalSent]), curBufferSize, tt);
                        else
                            stream.write((OUT_TYPE*)&((*transformedBuffer)[totalSent]), curBufferSize, tt);
                    }
                } else if (fftType > 0 && (typeid (OUT_TYPE) == typeid (double))) { // all FFT conversions to double
                    if (normalize_floating_point.output) {
                        if (fftType == 1)
                            float2doubleScaled((double*) &outputBuffer[0], (float*) &transformedRBuffer[totalSent], curBufferSize, sMin, dRange, sRange, dMin);
                        else
                            float2doubleScaled((double*) &outputBuffer[0], (float*) &(*transformedBuffer)[totalSent], curBufferSize, sMin, dRange, sRange, dMin);
                        stream.write((OUT_TYPE*) & outputBuffer[0], curBufferSize, tt);
                    } else {
                        if (fftType == 1)
                            float2double((double*) &outputBuffer[0], (float*) &transformedRBuffer[totalSent], curBufferSize);
                        else
                            float2double((double*) &outputBuffer[0], (float*) &(*transformedBuffer)[totalSent], curBufferSize);
                        stream.write((OUT_TYPE*) & outputBuffer[0], curBufferSize, tt);
                    }
                } else if (fftType > 0) {// && !isFloatingType<OUT_TYPE>()) { // all FFT conversions to non-floating-point
                    LOG_DEBUG(DataConverter_i,"pushDataService|FFT conversion to non-floating-point");
                    if (fftType == 1) {
                        dataTypeTransformOpt::convertDataType<float, OUT_TYPE>((float*) &transformedRBuffer[totalSent], (OUT_TYPE*) & outputBuffer[0], scaled, normalize_floating_point.input, false, curBufferSize);
                    } else {
                        LOG_DEBUG(DataConverter_i,"pushDataService|R2C conversion to non-float");
                        dataTypeTransformOpt::convertDataType<float, OUT_TYPE>((float*) &((*transformedBuffer)[totalSent]), (OUT_TYPE*) & outputBuffer[0], scaled, normalize_floating_point.input, false, curBufferSize);
                    }
                    LOG_DEBUG(DataConverter_i,"pushDataService|FFT to non-float - writing out curBufferSize="<<curBufferSize);
                    stream.write((OUT_TYPE*) & outputBuffer[0], curBufferSize, tt);
                    LOG_DEBUG(DataConverter_i,"pushDataService|FFT to non-float - DONE writing out curBufferSize="<<curBufferSize);
                } else if (typeid (IN_TYPE) == typeid (double)) { // all non-FFT conversions from double
                    dataTypeTransformOpt::convertDataType<IN_TYPE, float>((IN_TYPE*)&data[totalSent], (float*) &conversionBuffer[0], false, normalize_floating_point.input, normalize_floating_point.output, curBufferSize);
                    if (!normalize_floating_point.input) {
                        dataTypeTransformOpt::convertDataTypeRange<float, OUT_TYPE>((float*) &conversionBuffer[0], (OUT_TYPE*) & outputBuffer[0], sMin, sRange, dMin, dRange, curBufferSize);
                    } else {
                        dataTypeTransformOpt::convertDataType<float, OUT_TYPE>((float*) &conversionBuffer[0], (OUT_TYPE*) & outputBuffer[0], scaled, normalize_floating_point.input, normalize_floating_point.output, curBufferSize);
                    }
                    stream.write((OUT_TYPE*) & outputBuffer[0], curBufferSize, tt);
                } else if (typeid (OUT_TYPE) == typeid (double)) { // non-FFT conversions not from double, but to double
                    if (isFloatingType<IN_TYPE>() && !normalize_floating_point.input) {
                        dataTypeTransformOpt::convertDataTypeRange<IN_TYPE, float>((IN_TYPE*)&data[totalSent], (float*) & conversionBuffer[0], sMin, sRange, dMin, dRange, curBufferSize);
                    } else {
                        dataTypeTransformOpt::convertDataType<IN_TYPE, float>((IN_TYPE*)&data[totalSent], (float*) & conversionBuffer[0], scaled, normalize_floating_point.input, normalize_floating_point.output, curBufferSize);
                    }
                    float2double((double*) &outputBuffer[0], (float*) &conversionBuffer[totalSent], curBufferSize);
                    stream.write((OUT_TYPE*) & outputBuffer[0], curBufferSize, tt);
                } else { // all non-FFT conversions not from or to double
                    if (isFloatingType<IN_TYPE>() && !normalize_floating_point.input) {
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
        //bulkio::InShortStream stream = dataShort_in->getCurrentStream(0);
        if (!stream) {
            return false; // NOOP
        }

        typename IN_PORT_TYPE::StreamType::DataBlockType block = stream.read();
        // TODO - should read up to "max transfer size" to make sure we have enough memory allocated
        //      - account for complex vs scalar input and how that affects how many samples are read
        //bulkio::ShortDataBlock block = stream.read();
        if (!block) {
            if (stream.eos()) {
                // TODO eos stuff
                return true; // NORMAL
            }
            return false; // NOOP
        }

        if (block.inputQueueFlushed()) {
            LOG_WARN(DataConverter_i, "Input queue flushed.  Flushing internal buffers.");
            // TODO flush all our processor states if the queue flushed
            //flush();
        }

        if (block.sriChanged() || !currSRIPtr) {
            configureSRI(block.sri());
            // currSRIPtr now is a pointer to resulting sri

        }

        if (block.getStartTime().tcstatus == BULKIO::TCS_VALID && fftType != 0) {
            adjustTimeStamp(block.getStartTime(), _timestamp);
//            _samplesSinceLastTimestamp = 0;
        } else if (block.getStartTime().tcstatus == BULKIO::TCS_VALID) {
            _timestamp = block.getStartTime();
        } else if (block.getStartTime().tcstatus != BULKIO::TCS_VALID) {
            LOG_WARN(DataConverter_i, "Invalid, T.tcstatus Sending invalid timecode anyway. tcstatus:" << block.getStartTime().tcstatus);
            _timestamp = block.getStartTime();
        }

        redhawk::shared_buffer<typename IN_PORT_TYPE::dataTransfer::DataBufferType::value_type> dataBuffer = block.buffer();
        pushDataService<signed char>(dataBuffer, dataChar_out, stream.eos(), _timestamp, stream.streamID(), scaleOutput.charPort);
        pushDataService<unsigned char>(dataBuffer, dataOctet_out, stream.eos(), _timestamp, stream.streamID(), scaleOutput.octetPort);//,normalize_floating_point.normalized_input);
        pushDataService<short>(dataBuffer, dataShort_out, stream.eos(), _timestamp, stream.streamID(), scaleOutput.shortPort); //,normalize_floating_point.normalized_input);
        pushDataService<unsigned short>(dataBuffer, dataUshort_out, stream.eos(), _timestamp, stream.streamID(), scaleOutput.ushortPort);//,normalize_floating_point.normalized_input);
        pushDataService<float>(dataBuffer, dataFloat_out, stream.eos(), _timestamp, stream.streamID(),normalize_floating_point.output);//,normalize_floating_point.normalized_output);
        pushDataService<double>(dataBuffer, dataDouble_out, stream.eos(), _timestamp, stream.streamID(),normalize_floating_point.output);//,normalize_floating_point.normalized_output);

        return true;
    }

};



#endif
