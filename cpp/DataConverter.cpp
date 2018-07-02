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

/**************************************************************************

    This is the component code. This file contains the child class where
    custom functionality can be added to the component. Custom
    functionality to the base class can be extended here. Access to
    the ports can also be done from this class

 **************************************************************************/

#include "DataConverter.h"

using namespace fftwf_thread_coordinator;

DataConverter_i::DataConverter_i(const char *uuid, const char *label) : 
DataConverter_base(uuid, label)
{
    outputBuffer = NULL;
    conversionBuffer = NULL;

    workingBuffer = NULL;
    transformedBuffer = NULL;
    transformedRBuffer = NULL;
    fftBuffer = NULL;
    fftRBuffer = NULL;
    r2c = NULL;
    c2c_r = NULL;
    c2c_f = NULL;
    c2c_r2 = NULL;

    tempBuffer = NULL;
    complexTempBuffer = NULL;
    r2cTempBuffer = NULL;

    upsampled = NULL;
    filter = NULL;

    transferOut = NULL;
    remainder_v = 0;
    count = 0;
    createFFT = true;
    createMEM = true;
    _writeIndex = 0;
    dataAmount = 0;
    sampleRate = 0;
    inputMode = 0;
    _outWriteIndex = 0;
    mixerPhase = 0.0;
    multiple = 0.0;
    maxSamplesIn = 0;
    numFilterTaps = 0;
    overlapAmount = 0;
    currSRIPtr = NULL;
    _transferSize = 0;
    overlapRemovalPosition = 0;
    _readIndex = 0;
    first_overlap = true;
    outputMode = 0;
    fftType = 0;
    even_odd = true;
    fftSize = 0;
    _loadedDataCount = 0;
}

DataConverter_i::~DataConverter_i()
{
    if (outputBuffer != NULL)
        _mm_free(outputBuffer);
    if (conversionBuffer != NULL )
        _mm_free(conversionBuffer);
    outputBuffer = NULL;
    conversionBuffer = NULL;

    if (fftBuffer != NULL)
        fftwf_free(fftBuffer);
    if (fftRBuffer != NULL)
        fftwf_free(fftRBuffer);
    if (workingBuffer != NULL)
        fftwf_free(workingBuffer);
    if (transformedBuffer != NULL)
        fftwf_free(transformedBuffer);
    if (transformedRBuffer != NULL)
        fftwf_free(transformedRBuffer);
    if (upsampled != NULL)
        fftwf_free(upsampled);
    if (tempBuffer  != NULL)
        fftwf_free(tempBuffer);
    if (complexTempBuffer != NULL)
        fftwf_free(complexTempBuffer);
    if (r2cTempBuffer  != NULL)
        fftwf_free(r2cTempBuffer);
    fftBuffer = NULL;
    fftRBuffer = NULL;
    workingBuffer = NULL;
    transformedBuffer = NULL;
    transformedRBuffer = NULL;
    upsampled = NULL;
    tempBuffer = NULL;
    complexTempBuffer = NULL;
    r2cTempBuffer = NULL;

    {
        if (r2c != NULL) {
            boost::mutex::scoped_lock lock(getCoordinator()->getPlanMutex());
            RH_DEBUG(_baseLog,"destuctor: destroying r2c (A)");
            fftwf_destroy_plan(r2c);
            r2c = NULL;
        }
        if (c2c_r != NULL) {
            boost::mutex::scoped_lock lock(getCoordinator()->getPlanMutex());
            RH_DEBUG(_baseLog,"destuctor: destroying c2c_r (B)");
            fftwf_destroy_plan(c2c_r);
            c2c_r = NULL;
        }
        if (c2c_f != NULL) {
            boost::mutex::scoped_lock lock(getCoordinator()->getPlanMutex());
            RH_DEBUG(_baseLog,"destuctor: destroying c2c_f (C)");
            fftwf_destroy_plan(c2c_f);
            c2c_f = NULL;
        }
        if (c2c_r2 != NULL) {
            boost::mutex::scoped_lock lock(getCoordinator()->getPlanMutex());
            RH_DEBUG(_baseLog,"destuctor: destroying c2c_r2 (D)");
            fftwf_destroy_plan(c2c_r2);
            c2c_r2 = NULL;
        }
        if (filter  != NULL) {
            RH_DEBUG(_baseLog,"destuctor: destroying R2C filter (E)");
            delete filter;
            filter = NULL;
        }
    }
}

void DataConverter_i::constructor(){
    //std::string baseLogName  = _baseLog->getName();
    singleService_log        = _baseLog->getChildLogger("singleService");
    pushDataService_log      = _baseLog->getChildLogger("pushDataService");
    R2C_log                  = _baseLog->getChildLogger("R2C");
    dataTypeTransformOpt_log = _baseLog->getChildLogger("dataTypeTransformOpt");
    RH_DEBUG(_baseLog, " Testing _baseLog with name "<<_baseLog->getName());
    RH_DEBUG(singleService_log, " Testing singleService_log");
    RH_DEBUG(pushDataService_log, " Testing pushDataService_log");
    RH_DEBUG(R2C_log, " Testing R2C_log");
    RH_DEBUG(dataTypeTransformOpt_log, " Testing dataTypeTransformOpt_log");

    first_overlap = true;
    remainder_v = 0;

    count = 0;

    _readIndex = 0;
    _writeIndex = 0;

    inputMode = 0;
    outputMode = 0;
    numFilterTaps = 0;

    fftType = 1; // C2R
    //fftw_init_threads();
    //fftw_plan_with_nthreads(FFTW_NUMBER_THREADS);
    mixerPhase = 0;

    currSRIPtr = 0;

    maxSamplesIn = 0;
    _transferSize = maxTransferSize;
    createMEM = true;
    createFFT = true;

    addPropertyListener(transformProperties, this, &DataConverter_i::transformPropertiesChanged);
    addPropertyListener(maxTransferSize, this, &DataConverter_i::maxTransferSizeChanged);
    addPropertyListener(outputType, this, &DataConverter_i::outputTypeChanged);
}

void DataConverter_i::createMemory(int bufferSize){
    RH_TRACE(_baseLog,"createMemory (enter) bufferSize="<<bufferSize);
    if (outputBuffer != NULL)
        _mm_free(outputBuffer);
    if (conversionBuffer != NULL)
        _mm_free(conversionBuffer);

    //create a buffer that is used between all types
    //largest output buffer requirement is for double
    //largest conversion buffer requirement is for float
    //created for 16byte aligned data
    outputBuffer = (char*) _mm_malloc(sizeof(double)*bufferSize,16);
    conversionBuffer = (char*) _mm_malloc(sizeof(float)*bufferSize,16);
    createMEM = false;
    createFFT = true;
}

void DataConverter_i::createFFTroute(int bufferSize){
    RH_TRACE(_baseLog,"createFFTroute (enter) bufferSize="<<bufferSize);
    if (fftBuffer != NULL)
        fftwf_free(fftBuffer);
    if (fftRBuffer != NULL)
        fftwf_free(fftRBuffer);
    if (tempBuffer  != NULL)
        fftwf_free(tempBuffer);
    if (complexTempBuffer != NULL)
        fftwf_free(complexTempBuffer);
    if (r2cTempBuffer  != NULL)
        fftwf_free(r2cTempBuffer);
    if (workingBuffer != NULL)
        fftwf_free(workingBuffer);
    if (transformedBuffer != NULL)
        fftwf_free(transformedBuffer);
    if (transformedRBuffer != NULL)
        fftwf_free(transformedRBuffer);
    if (upsampled != NULL)
        fftwf_free(upsampled);
    {
        if (r2c != NULL) {
            RH_TRACE(_baseLog,"createFFTroute destroy r2c");
            boost::mutex::scoped_lock lock(getCoordinator()->getPlanMutex());
            RH_DEBUG(_baseLog,"createFFTroute: destroying r2c (A)");
            fftwf_destroy_plan(r2c);
            r2c = NULL;
        }
        if (c2c_r != NULL) {
            RH_TRACE(_baseLog,"createFFTroute destroy c2c_r");
            boost::mutex::scoped_lock lock(getCoordinator()->getPlanMutex());
            RH_DEBUG(_baseLog,"createFFTroute: destroying c2c_r (B)");
            fftwf_destroy_plan(c2c_r);
            c2c_r = NULL;
        }
        if (c2c_f != NULL) {
            RH_TRACE(_baseLog,"createFFTroute destroy c2c_f");
            boost::mutex::scoped_lock lock(getCoordinator()->getPlanMutex());
            RH_DEBUG(_baseLog,"createFFTroute: destroying c2c_f (C)");
            fftwf_destroy_plan(c2c_f);
            c2c_f = NULL;
        }
        if (c2c_r2 != NULL) {
            RH_TRACE(_baseLog,"createFFTroute destroy c2c_r2");
            boost::mutex::scoped_lock lock(getCoordinator()->getPlanMutex());
            RH_DEBUG(_baseLog,"createFFTroute: destroying c2c_r2 (D)");
            fftwf_destroy_plan(c2c_r2);
            c2c_r2 = NULL;
        }
    }

    //initialize all pointers to NULL
    fftBuffer = NULL;
    fftRBuffer = NULL;
    tempBuffer = NULL;
    complexTempBuffer = NULL;
    r2cTempBuffer = NULL;
    workingBuffer = NULL;
    transformedBuffer = NULL;
    transformedRBuffer = NULL;
    upsampled = NULL;

    //used in R2C and C2R
    fftBuffer = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*transformProperties.fftSize);
    fftRBuffer = (float*) fftwf_malloc(sizeof(float)*transformProperties.fftSize*4);
    RH_TRACE(_baseLog,"createFFTroute fftBuffer addr is "<<(float*) fftBuffer);
    RH_TRACE(_baseLog,"createFFTroute fftRBuffer addr is "<<fftRBuffer);

    if (fftType == 2){ // R2C
        RH_TRACE(_baseLog,"createFFTroute|R2C");
        complexTempBuffer = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*transformProperties.fftSize);
        r2cTempBuffer = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*transformProperties.fftSize);
        workingBuffer = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*bufferSize);
        transformedBuffer  = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*bufferSize);
        {
            boost::mutex::scoped_lock lock(getCoordinator()->getPlanMutex());
            RH_DEBUG(_baseLog,"createFFTroute: creating r2c (A) - fftwf_plan_dft_r2c_1d size="<<transformProperties.fftSize);
            r2c = fftwf_plan_dft_r2c_1d(transformProperties.fftSize, (float*)&fftBuffer[0], workingBuffer, MY_FFTW_FLAGS);
            RH_DEBUG(_baseLog,"createFFTroute: creating c2c_r (B) - fftwf_plan_dft_1d BWD size="<<transformProperties.fftSize);
            c2c_r = fftwf_plan_dft_1d(transformProperties.fftSize, r2cTempBuffer, complexTempBuffer, FFTW_BACKWARD, MY_FFTW_FLAGS);
        }
        RH_TRACE(_baseLog,"createFFTroute|R2C complexTempBuffer addr is "<<(float*)complexTempBuffer);
        RH_TRACE(_baseLog,"createFFTroute|R2C r2cTempBuffer addr is "<<(float*)r2cTempBuffer);
        RH_TRACE(_baseLog,"createFFTroute|R2C workingBuffer addr is "<<(float*)workingBuffer);
        RH_TRACE(_baseLog,"createFFTroute|R2C r2c addr is "<<(void*)r2c);
        RH_TRACE(_baseLog,"createFFTroute|R2C c2c_r addr is "<<(void*)c2c_r);
    } else {
        RH_TRACE(_baseLog,"createFFTroute|C2R");
        upsampled = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*transformProperties.fftSize*2);
        complexTempBuffer = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*transformProperties.fftSize*2);
        transformedRBuffer = (float*)fftwf_malloc(sizeof(float)*bufferSize*2);
        workingBuffer = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*transformProperties.fftSize);
        transformedBuffer  = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*transformProperties.fftSize);
        {
            boost::mutex::scoped_lock lock(getCoordinator()->getPlanMutex());
            RH_DEBUG(_baseLog,"createFFTroute: creating c2c_f (C) - fftwf_plan_dft_1d FWD size="<<transformProperties.fftSize);
            c2c_f  = fftwf_plan_dft_1d(transformProperties.fftSize, fftBuffer, workingBuffer, FFTW_FORWARD, MY_FFTW_FLAGS);
            RH_DEBUG(_baseLog,"createFFTroute: creating c2c_r2 (D) - fftwf_plan_dft_1d BWD size="<<transformProperties.fftSize*2);
            c2c_r2 = fftwf_plan_dft_1d(transformProperties.fftSize*2, upsampled, complexTempBuffer, FFTW_BACKWARD, MY_FFTW_FLAGS);
        }
        RH_TRACE(_baseLog,"createFFTroute|R2C upsampled addr is "<<(float*)upsampled);
        RH_TRACE(_baseLog,"createFFTroute|R2C complexTempBuffer addr is "<<(float*)complexTempBuffer);
        RH_TRACE(_baseLog,"createFFTroute|R2C transformedRBuffer addr is "<<(float*)transformedRBuffer);
        RH_TRACE(_baseLog,"createFFTroute|R2C workingBuffer addr is "<<(float*)workingBuffer);
        RH_TRACE(_baseLog,"createFFTroute|R2C transformedBuffer addr is "<<(float*)transformedBuffer);
        RH_TRACE(_baseLog,"createFFTroute|R2C c2c_f addr is "<<(void*)c2c_f);
        RH_TRACE(_baseLog,"createFFTroute|R2C c2c_r2 addr is "<<(void*)c2c_r2);
    }
    createFFT = false;
    RH_DEBUG(_baseLog,"createFFTroute: creating R2C filter (E)");
    createFilter();
}

void DataConverter_i::transformPropertiesChanged(const transformProperties_struct &oldVal, const transformProperties_struct &newVal) {
    RH_TRACE(_baseLog,__PRETTY_FUNCTION__ << "oldVal.fftSize="<<oldVal.fftSize<<" newVal.fftSize="<<newVal.fftSize);
    boost::mutex::scoped_lock lock(property_lock);

    // We only care if the fftSize changes
    if (oldVal.fftSize == newVal.fftSize) {
        return;
    }
    RH_DEBUG(_baseLog," New fftSize configured: "<<newVal.fftSize);
    createFFT = true;
}

void DataConverter_i::maxTransferSizeChanged(CORBA::Long oldVal, CORBA::Long newVal) {
    RH_TRACE(_baseLog,__PRETTY_FUNCTION__ << "oldVal="<<oldVal<<" newVal="<<newVal);
    boost::mutex::scoped_lock lock(property_lock);

    if (maxTransferSize >= 0) {
        RH_DEBUG(_baseLog," Configured with maxTransferSize: "<<maxTransferSize);
        if (maxTransferSize != 0) {
            _transferSize = maxTransferSize;
        }
        createMEM = true;
    }
}

void DataConverter_i::outputTypeChanged(short oldVal, short newVal) {
    RH_TRACE(_baseLog,__PRETTY_FUNCTION__ << "oldVal="<<oldVal<<" newVal="<<newVal);
    boost::mutex::scoped_lock lock(property_lock);
    RH_DEBUG(_baseLog," configured with outputType: "<<outputType);
    RH_TRACE(_baseLog,"outputTypeChanged|before: inputMode="<<inputMode<<" outputMode="<<outputMode<<" outputType="<<outputType<<" fftType="<<fftType);

    // Reconfigure current SRI to push out mode change if we have a current SRI
    if (currSRIPtr) {
        RH_TRACE(_baseLog,"outputTypeChanged|configuring sri; sri.mode="<<currSRIPtr->mode);
        configureSRI(*currSRIPtr, false);
        RH_TRACE(_baseLog,"outputTypeChanged|after: sri.mode="<<currSRIPtr->mode);
    }
    RH_TRACE(_baseLog,"outputTypeChanged|after: inputMode="<<inputMode<<" outputMode="<<outputMode<<" outputType="<<outputType<<" fftType="<<fftType);
}

void DataConverter_i::createFilter(){
    RH_TRACE(_baseLog,"createFilter (enter)");
    if(filter != NULL) {
        delete filter;
        filter = NULL;
    }
    size_t numberTaps = (transformProperties.fftSize*transformProperties.overlap_percentage/100)+1;
    RH_TRACE(_baseLog,"createFilter|1  numberTaps="<<numberTaps<<" numFilterTaps="<<numFilterTaps);
    if(!(numberTaps%2))
        numberTaps = numberTaps-1;
    RH_TRACE(_baseLog,"createFilter|2  numberTaps="<<numberTaps<<" numFilterTaps="<<numFilterTaps);
    numFilterTaps = numberTaps;
    RH_TRACE(_baseLog,"createFilter|3  numberTaps="<<numberTaps<<" numFilterTaps="<<numFilterTaps);

    filter = new R2C(transformProperties.lowCutoff, transformProperties.highCutoff, transformProperties.transitionWidth, numberTaps, transformProperties.fftSize, R2C_log);

    RH_TRACE(_baseLog,"createFilter filter addr is "<<(void*)filter);
}
/***********************************************************************************************

    Basic functionality:

        The service function is called by the serviceThread object (of type ProcessThread).
        This call happens immediately after the previous call if the return value for
        the previous call was NORMAL.
        If the return value for the previous call was NOOP, then the serviceThread waits
        an amount of time defined in the serviceThread's constructor.

    SRI:
        To create a StreamSRI object, use the following code:
            stream_id = "";
            sri = BULKIO::StreamSRI();
            sri.hversion = 1;
            sri.xstart = 0.0;
            sri.xdelta = 0.0;
            sri.xunits = BULKIO::UNITS_TIME;
            sri.subsize = 0;
            sri.ystart = 0.0;
            sri.ydelta = 0.0;
            sri.yunits = BULKIO::UNITS_NONE;
            sri.mode = 0;
            sri.streamID = this->stream_id.c_str();

    Time:
        To create a PrecisionUTCTime object, use the following code:
            struct timeval tmp_time;
            struct timezone tmp_tz;
            gettimeofday(&tmp_time, &tmp_tz);
            double wsec = tmp_time.tv_sec;
            double fsec = tmp_time.tv_usec / 1e6;;
            BULKIO::PrecisionUTCTime tstamp = BULKIO::PrecisionUTCTime();
            tstamp.tcmode = BULKIO::TCM_CPU;
            tstamp.tcstatus = (short)1;
            tstamp.toff = 0.0;
            tstamp.twsec = wsec;
            tstamp.tfsec = fsec;

    Ports:

        Data is passed to the serviceFunction through the getPacket call (BULKIO only).
        The dataTransfer class is a port-specific class, so each port implementing the
        BULKIO interface will have its own type-specific dataTransfer.

        The argument to the getPacket function is a floating point number that specifies
        the time to wait in seconds. A zero value is non-blocking. A negative value
        is blocking.

        Each received dataTransfer is owned by serviceFunction and *MUST* be
        explicitly deallocated.

        To send data using a BULKIO interface, a convenience interface has been added 
        that takes a std::vector as the data input

        NOTE: If you have a BULKIO dataSDDS port, you must manually call 
              "port->updateStats()" to update the port statistics when appropriate.

        Example:
            // this example assumes that the component has two ports:
            //  A provides (input) port of type BULKIO::dataShort called short_in
            //  A uses (output) port of type BULKIO::dataFloat called float_out
            // The mapping between the port and the class is found
            // in the component base class header file

            BULKIO_dataShort_In_i::dataTransfer *tmp = short_in->getPacket(-1);
            if (not tmp) { // No data is available
                return NOOP;
            }

            std::vector<float> outputData;
            outputData.resize(tmp->dataBuffer.size());
            for (unsigned int i=0; i<tmp->dataBuffer.size(); i++) {
                outputData[i] = (float)tmp->dataBuffer[i];
            }

            // NOTE: You must make at least one valid pushSRI call
            if (tmp->sriChanged) {
                float_out->pushSRI(tmp->SRI);
            }
            float_out->pushPacket(outputData, tmp->T, tmp->EOS, tmp->streamID);

            delete tmp; // IMPORTANT: MUST RELEASE THE RECEIVED DATA BLOCK
            return NORMAL;

        Interactions with non-BULKIO ports are left up to the component developer's discretion

    Properties:

        Properties are accessed directly as member variables. For example, if the
        property name is "baudRate", it may be accessed within member functions as
        "baudRate". Unnamed properties are given a generated name of the form
        "prop_n", where "n" is the ordinal number of the property in the PRF file.
        Property types are mapped to the nearest C++ type, (e.g. "string" becomes
        "std::string"). All generated properties are declared in the base class
        (DataConverter_base).

        Simple sequence properties are mapped to "std::vector" of the simple type.
        Struct properties, if used, are mapped to C++ structs defined in the
        generated file "struct_props.h". Field names are taken from the name in
        the properties file; if no name is given, a generated name of the form
        "field_n" is used, where "n" is the ordinal number of the field.

        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue
            //  - A boolean called scaleInput

            if (scaleInput) {
                dataOut[i] = dataIn[i] * scaleValue;
            } else {
                dataOut[i] = dataIn[i];
            }

        A callback method can be associated with a property so that the method is
        called each time the property value changes.  This is done by calling 
        setPropertyChangeListener(<property name>, this, &DataConverter::<callback method>)
        in the constructor.

        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue

        //Add to DataConverter.cpp
        DataConverter_i::DataConverter_i(const char *uuid, const char *label) :
            DataConverter_base(uuid, label)
        {
            setPropertyChangeListener("scaleValue", this, &DataConverter_i::scaleChanged);
        }

        void DataConverter_i::scaleChanged(const std::string& id){
            std::cout << "scaleChanged scaleValue " << scaleValue << std::endl;
        }

        //Add to DataConverter.h
        void scaleChanged(const std::string&);


 ************************************************************************************************/
int DataConverter_i::serviceFunction()
{
    bool retService;
    retService = singleService(dataChar_in);
    retService = retService || singleService(dataOctet_in);
    retService = retService || singleService(dataShort_in);
    retService = retService || singleService(dataUshort_in);
    retService = retService || singleService(dataFloat_in);
    retService = retService || singleService(dataDouble_in);

    if (retService) {
        return NORMAL;
    }
    return NOOP;
}



int DataConverter_i::calculate_memorySize(int inputAmount){
    RH_TRACE(_baseLog,"calculate_memorySize|inputAmount="<<inputAmount);
    int value = 0;
    int size = 0;
    if (fftType > 0) { // R2C or C2R
        value = ceil(inputAmount/((transformProperties.fftSize * transformProperties.overlap_percentage/100)));
        size = value*transformProperties.fftSize;
    } else {
        size = inputAmount;
    }
    if (maxTransferSize == 0)
        _transferSize = size;
    RH_TRACE(_baseLog,"calculate_memorySize|result="<<size);
    return size;
}

void DataConverter_i::setupFFT(){
    RH_TRACE(_baseLog,"setupFFT");
    int oldFftType = fftType;
    if (inputMode == 0 && outputType == 2) { // Real input, Complex output
        fftType = 2;    // R2C
        outputMode = 1; // Complex
    } else if (inputMode == 1 && outputType == 1) { // Complex input, Real output
        fftType = 1;    // C2R
        outputMode = 0; // Real
    } else { // passive (outputType==0 or outputType matches inputMode)
        fftType = 0;
        outputMode = inputMode;
    }
    if (fftType != oldFftType) {
        RH_DEBUG(_baseLog,"setupFFT - fftType changed from "<<oldFftType<<" to "<<fftType);
        createMEM = true;
        createFFT = true;
    }
}

void DataConverter_i::configureSRI(const BULKIO::StreamSRI& sriIn, bool incomingSRI) {
    RH_TRACE(_baseLog,"configureSRI|incomingSRI="<<incomingSRI);
    sampleRate = 1 / sriIn.xdelta;
    // Only set the input mode if the SRI is incoming
    if (incomingSRI)
        inputMode = sriIn.mode;
    setupFFT();

    double cf_offset = 0;
    if (fftType == 1) { // C2R
        sampleRate = 2*sampleRate;
        cf_offset = sampleRate/4;
    } else if (fftType == 2) { // R2C
        if (transformProperties.tune_fs_over_4) {
            cf_offset = -sampleRate/4;
            sampleRate = sampleRate/2;
        }
    }

    BULKIO::StreamSRI* sriOutPtr = new BULKIO::StreamSRI(sriIn);
    sriOutPtr->xdelta = 1.0/sampleRate;

    for (unsigned int i = 0; i < sriOutPtr->keywords.length(); ++i){
        if(std::string(sriOutPtr->keywords[i].id) == "COL_RF" && advancedSRI.update_col_rf_keyword){
            CORBA::Double COL_RF = 0;
            sriOutPtr->keywords[i].value >>= COL_RF;
            COL_RF += cf_offset;
            sriOutPtr->keywords[i].value <<= COL_RF;
        }
        else if(std::string(sriOutPtr->keywords[i].id) == "CHAN_RF" && advancedSRI.update_chan_rf_keyword){
            CORBA::Double CHAN_RF = 0;
            sriOutPtr->keywords[i].value >>= CHAN_RF;
            CHAN_RF += cf_offset;
            sriOutPtr->keywords[i].value <<= CHAN_RF;
        }
    }
    sriOutPtr->mode = outputMode;
    dataChar_out->createStream(*sriOutPtr);
    dataOctet_out->createStream(*sriOutPtr);
    dataShort_out->createStream(*sriOutPtr);
    dataUshort_out->createStream(*sriOutPtr);
    dataFloat_out->createStream(*sriOutPtr);
    dataDouble_out->createStream(*sriOutPtr);

    // Save off copy of current SRI clean up previous SRI
    BULKIO::StreamSRI* lastSRIPtr = currSRIPtr;
    currSRIPtr = sriOutPtr;
    if (lastSRIPtr) {
        delete lastSRIPtr;
    }
}

int DataConverter_i::fft_execute(float* data, int size, bool EOS) {
    RH_TRACE(_baseLog,"fft_execute|enter sample0="<<data[0]);
    _readIndex = 0;
    _writeIndex = 0;

    const float multiple = 1.0/transformProperties.fftSize;

    int elements_per_sample_input = 1;
    int elements_per_sample_output = 1;
    if (inputMode)
        elements_per_sample_input = 2;
    if (outputMode)
        elements_per_sample_output = 2;
    int dataAmount = 0;
    bool real_to_complex = (fftType == 2);


    int overlap = transformProperties.fftSize * transformProperties.overlap_percentage/100;
    if (!real_to_complex)
        overlap = 0;
    int overlapRemovalPosition = (transformProperties.fftSize - overlap);

    if (EOS && size <= 0 && (first_overlap || remainder_v <= overlap*elements_per_sample_input)){
        first_overlap = true;
        return 0;
    }

    // on a new stream, zero out overlap in buffer
    if (first_overlap){
        memset(&fftBuffer[0],0,sizeof(float)*overlap*elements_per_sample_input);
        //set the input to start copy at the overlap position
        remainder_v = overlap * elements_per_sample_input;
        first_overlap = false;
    }

    int size_avail = size;
    float* fftBuffRealPtr = (float*) &fftBuffer[0];

    if (remainder_v != 0)
        moveTimeStamp(remainder_v);
    do{
        int elements_to_process = std::min(size_avail,int(transformProperties.fftSize*elements_per_sample_input)-remainder_v);
        //move data into processing buffer
        if (elements_to_process > 0){
            memcpy(&fftBuffRealPtr[remainder_v],&data[_readIndex],sizeof(float)*elements_to_process);
        }

        size_avail -= elements_to_process;
        _readIndex += elements_to_process;
        remainder_v += elements_to_process;

        //fill out incomplete transform with zeros
        int num_zeros = transformProperties.fftSize * elements_per_sample_input - remainder_v;
        if (EOS && (num_zeros > 0)){
            memset(&fftBuffRealPtr[remainder_v],0,sizeof(float)*num_zeros);
            remainder_v += num_zeros;
        }

        //perform fft
        if (remainder_v >= transformProperties.fftSize * elements_per_sample_input){

            if (real_to_complex){
                RH_TRACE(_baseLog,"fft_execute|real_to_complex fftwf_execute_dft_r2c(r2c) (A)");
                memcpy(&fftRBuffer[0],(float*)&fftBuffRealPtr[overlapRemovalPosition*elements_per_sample_input],sizeof(float)*overlap*elements_per_sample_input);
                fftwf_execute_dft_r2c(r2c,(float*)&fftBuffer[0],workingBuffer);
                RH_TRACE(_baseLog,"fft_execute|real_to_complex before filter sample0="<<((float*)workingBuffer)[0]);
                filter->workFreq((fftwf_complex*)&workingBuffer[0],(fftwf_complex*)&workingBuffer[transformProperties.fftSize/2],(fftwf_complex*)&r2cTempBuffer[0]);
                RH_TRACE(_baseLog,"fft_execute|real_to_complex after filter sample0="<<((float*)r2cTempBuffer)[0]);
            } else {
                RH_TRACE(_baseLog,"fft_execute|complex_to_real fftwf_execute_dft(c2c_f) (C)");
                fftwf_execute_dft(c2c_f,(fftwf_complex*)&fftBuffer[0],&workingBuffer[0]);
                mul_const((float*)&fftBuffer[0],(const float*) &workingBuffer[0],multiple*0.5,transformProperties.fftSize*2);
                memcpy(&upsampled[0],&fftBuffer[transformProperties.fftSize/2],sizeof(fftwf_complex)*transformProperties.fftSize/2);
                memcpy(&upsampled[transformProperties.fftSize/2],&fftBuffer[0],sizeof(fftwf_complex)*transformProperties.fftSize/2);
                for (CORBA::Long ii=transformProperties.fftSize, jj=transformProperties.fftSize/2-1; ii<3*transformProperties.fftSize/2-1; ++ii, --jj)
                {
                    upsampled[ii+1][0] = fftBuffer[jj][0];
                    upsampled[ii+1][1] = -1*fftBuffer[jj][1];
                }
                for (CORBA::Long ii=3*transformProperties.fftSize/2, jj=transformProperties.fftSize-1; ii<transformProperties.fftSize*2-1; ++ii, --jj)
                {
                    upsampled[ii+1][0] = fftBuffer[jj][0];
                    upsampled[ii+1][1] = -1*fftBuffer[jj][1];
                }
            }
            if (real_to_complex){
                RH_TRACE(_baseLog,"fft_execute|real_to_complex fftwf_execute_dft(c2c_r) (B)");
                RH_DEBUG(_baseLog,"fft_execute|real_to_complex before fftwf_execute_dft sample0="<<((float*)r2cTempBuffer)[0]);
                fftwf_execute_dft(c2c_r,r2cTempBuffer,complexTempBuffer);
                RH_DEBUG(_baseLog,"fft_execute|real_to_complex after fftwf_execute_dft sample0="<<((float*)complexTempBuffer)[0]);
                RH_DEBUG(_baseLog,"fft_execute|real_to_complex after fftwf_execute_dft sample[overlap]="<<((float*)complexTempBuffer)[overlap*2]);


                if(transformProperties.tune_fs_over_4){
                    RH_DEBUG(_baseLog,"fft_execute|real_to_complex tune_fs_over_4");
                    //std::cout << " divide by 2 case " << std::endl;
                    //mixing by (Fs/4)/Fs = 1/4 * dec in the time domain
                    //1/4*2 = 1/2 or 0.5
                    float mixerMod = 0.5;
                    std::complex<float> mixer;
                    int index = _writeIndex;
                    //int index = 0; //always a power of two in this case, so we always start at the same place...i think
                    for (int i = overlap;i<transformProperties.fftSize;i+=2){
                        //std::cout <<" sampleRate is "<< sampleRate << " sampleRate/2 is " << sampleRate/2.0 <<"MixerPhase is " << mixerPhase << " mixerMod is " << mixerMod << std::endl;
                        mixer = exp(std::complex<float>(0,-2*M_PI*mixerPhase));
                        //std::cout << " index is " << mixer << std::endl;
                        transformedBuffer[index][0] = complexTempBuffer[i][0] * mixer.real() - complexTempBuffer[i][1] * mixer.imag();
                        transformedBuffer[index][1] = complexTempBuffer[i][0] * mixer.imag() + complexTempBuffer[i][1] * mixer.real();
                        mixerPhase=fmod(mixerPhase + mixerMod,1);

                        index++;
                    }
                    RH_DEBUG(_baseLog,"fft_execute|real_to_complex after tune_fs_over_4 sample0="<<((float*)transformedBuffer)[_writeIndex*2]);
                    //fwrite((fftwf_complex*)&transformedBuffer[_writeIndex],sizeof(fftwf_complex),overlapRemovalPosition/2,debugR2C);
                    //set size of _writeIndex
                    _writeIndex += overlapRemovalPosition/2;
                    dataAmount += overlapRemovalPosition/2 * elements_per_sample_output;
                }else{
                    RH_DEBUG(_baseLog,"fft_execute|real_to_complex (no tune)");

                    memcpy(&transformedBuffer[_writeIndex],&complexTempBuffer[overlap],sizeof(float)*overlapRemovalPosition*elements_per_sample_output);
                    RH_DEBUG(_baseLog,"fft_execute|real_to_complex after (no tune) sample0="<<((float*)transformedBuffer)[_writeIndex*2]);
                    _writeIndex += overlapRemovalPosition;
                    dataAmount += overlapRemovalPosition * elements_per_sample_output;
                }
            }else{
                RH_TRACE(_baseLog,"fft_execute|complex_to_real fftwf_execute_dft(c2c_r2) (D)");
                fftwf_execute_dft(c2c_r2,upsampled,complexTempBuffer);
                //extract the real only value (which we now have 2X of
                int index_l = _writeIndex;
                for (int i = 0;i<transformProperties.fftSize*2;++i){
                    transformedRBuffer[index_l] = complexTempBuffer[i][0];
                    index_l++;
                }
                _writeIndex += transformProperties.fftSize*2;
                dataAmount += transformProperties.fftSize*2;

            }
            memcpy((float*)&fftBuffer[0],&fftRBuffer[0],sizeof(float)*overlap*elements_per_sample_input);

            remainder_v = overlap*elements_per_sample_input;
        }

    }while(size_avail > 0);
    if (EOS)
        first_overlap = true;
    RH_TRACE(_baseLog,"fft_execute|exit dataAmount="<<dataAmount);
    return dataAmount;
}

void DataConverter_i::moveTimeStamp(int shiftBack){
    //save off this timestamp in case there is a remainder from the previous
    //new timestamp, but data still left in buffer from previous one
    //modify the timestamp to account for the first sample from the old buffer
    if (shiftBack != 0){
        double temp = 0;
        temp = (double)shiftBack * (1.0/sampleRate);
        _timestamp.tfsec = _timestamp.tfsec - temp;
        bulkio::time::utils::normalize(_timestamp);
    }
}

void DataConverter_i::adjustTimeStamp(const BULKIO::PrecisionUTCTime& timestamp, BULKIO::PrecisionUTCTime& localtimestamp){

    size_t numberTaps = (transformProperties.fftSize*transformProperties.overlap_percentage/100)+1;
    if(!(numberTaps%2))
        numberTaps = numberTaps-1;
    numFilterTaps = numberTaps;
    double grpDelay = ((double)numFilterTaps)/(2.0*sampleRate);
    localtimestamp = timestamp;
    //adjust the group delay
    localtimestamp.tfsec += grpDelay;
    bulkio::time::utils::normalize(localtimestamp);
}

void DataConverter_i::flush() {
    // Anything else need to be flushed when there's a gap in data?
    delete currSRIPtr;
    currSRIPtr=NULL;
    createFFT=true;
}
