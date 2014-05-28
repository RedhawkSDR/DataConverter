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

PREPARE_LOGGING(DataConverter_i)

DataConverter_i::DataConverter_i(const char *uuid, const char *label) : 
DataConverter_base(uuid, label)
{
	_constructor_();
}

DataConverter_i::~DataConverter_i()
{
	if (outputB != NULL)
		_mm_free(outputB);
	if (conversionBuffer != NULL )
		_mm_free(conversionBuffer);

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
	if (filter  != NULL)
		delete filter;

	if (r2c != NULL)
		fftwf_destroy_plan(r2c);
	if (c2c_r != NULL)
		fftwf_destroy_plan(c2c_r);
	if (c2c_f != NULL)
		fftwf_destroy_plan(c2c_f);
	if (c2c_r2 != NULL)
		fftwf_destroy_plan(c2c_r2);
	fftwf_cleanup();
}

void DataConverter_i::_constructor_(){
	inputMode = 0;
	first_overlap = true;
	firstRun = true;
	remainder_v = 0;
	outputB = NULL;
	conversionBuffer = NULL;

	workingBuffer = NULL;
	transformedBuffer = NULL;
	transformedRBuffer = NULL;
	fftBuffer = NULL;
	fftRBuffer = NULL;
	upsampled = NULL;
	r2c = NULL;
	c2c_r = NULL;
	c2c_f = NULL;
	c2c_r2 = NULL;

	count = 0;

	tempBuffer = NULL;
	complexTempBuffer = NULL;
	r2cTempBuffer = NULL;

	floatBuffer = NULL;
	complexBuffer = NULL;
	upsampled = NULL;
	filter = NULL;

	_transferSize = 0;
	createMEM = true;
	createFFT = true;
	//debugR2C = fopen("/tmp/debugfile.dat","wb");
	_samplesSinceLastTimestamp = 0;

	_readIndex = 0;
	_writeIndex = 0;

	inputMode = 0;
	outputMode = 0;
	numFilterTaps = 0;

	fftType = 1;
	//fftw_init_threads();
	//fftw_plan_with_nthreads(FFTW_NUMBER_THREADS);
	mixerPhase = 0;

    addPropertyChangeListener("transformProperties", this, &DataConverter_i::transformPropertiesChanged);
    addPropertyChangeListener("maxTransferSize", this, &DataConverter_i::maxTransferSizeChanged);
    
    _outDoublePort = new OutDoublePortUsingFloats("dataDouble_out");
    addPort("dataDouble_out", _outDoublePort);
    //ossie::corba::RootPOA()->activate_object(_outDoublePort);
    //registerOutPort(_outDoublePort, _outDoublePort->_this());
}

void DataConverter_i::createMemory(int bufferSize){
	if (outputB != NULL)
		_mm_free(outputB);
	if (conversionBuffer != NULL)
		_mm_free(conversionBuffer);
	//create a buffer that is used between all times
	//thus is must be a complex float in size

	//created for 16byte aligned data
	outputB = (char*) _mm_malloc(sizeof(float)*bufferSize,16);
	conversionBuffer = (char*) _mm_malloc(sizeof(float)*bufferSize,16);
	createMEM = false;
}

void DataConverter_i::createFFTroute(int bufferSize){
	//std::cout << "Call to createFFTroute " << bufferSize << " " << transformProperties.fftSize << std::endl;
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
	if (r2c != NULL)
		fftwf_destroy_plan(r2c);
	if (c2c_r != NULL)
		fftwf_destroy_plan(c2c_r);
	if (c2c_f != NULL)
		fftwf_destroy_plan(c2c_f);
	if (c2c_r2 != NULL)
		fftwf_destroy_plan(c2c_r2);

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
	r2c = NULL;
	c2c_r = NULL;
	c2c_f = NULL;
	c2c_r2 = NULL;


	//	floatBuffer = NULL;
	//	complexBuffer = NULL;
	//	upsampled = NULL;

	//used in R2C and C2R
	fftBuffer = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*transformProperties.fftSize);
	fftRBuffer = (float*) fftwf_malloc(sizeof(float)*transformProperties.fftSize*4);

	if (fftType == 2){
		complexTempBuffer = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*transformProperties.fftSize);
		r2cTempBuffer = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*transformProperties.fftSize);
		workingBuffer = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*bufferSize);
		transformedBuffer  = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*bufferSize);
		r2c = fftwf_plan_dft_r2c_1d(transformProperties.fftSize, (float*)&fftBuffer[0], workingBuffer, MY_FFTW_FLAGS);
		c2c_r = fftwf_plan_dft_1d(transformProperties.fftSize, r2cTempBuffer, complexTempBuffer, FFTW_BACKWARD, MY_FFTW_FLAGS);

	}else{
		upsampled = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*transformProperties.fftSize*2);
		complexTempBuffer = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*transformProperties.fftSize*2);
		transformedRBuffer = (float*)fftwf_malloc(sizeof(float)*bufferSize*2);
		workingBuffer = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*transformProperties.fftSize);
		transformedBuffer  = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*transformProperties.fftSize);
		c2c_f  = fftwf_plan_dft_1d(transformProperties.fftSize, fftBuffer, workingBuffer, FFTW_FORWARD, MY_FFTW_FLAGS);
		c2c_r2 = fftwf_plan_dft_1d(transformProperties.fftSize*2, upsampled, complexTempBuffer, FFTW_BACKWARD, MY_FFTW_FLAGS);
	}
	createFFT = false;
	createFilter();
}

void DataConverter_i::transformPropertiesChanged(const transformProperties_struct* oldVal,
                                                 const transformProperties_struct* newVal) {
    boost::mutex::scoped_lock lock(property_lock);

    // We only care if the fftSize changes
    if (oldVal->fftSize == newVal->fftSize) {
        return;
    }
    if ( (fftType != 0) || firstRun) {
        createFFT = true;   
    }
}

void DataConverter_i::maxTransferSizeChanged(const CORBA::Long* oldVal,
                                             const CORBA::Long* newVal) {
    boost::mutex::scoped_lock lock(property_lock);

    if ( (maxTransferSize >= 0) || firstRun) {
        if (maxTransferSize != 0) {
	    _transferSize = maxTransferSize;
        }
        createMEM = true;
    }
}

void DataConverter_i::createFilter(){
	if(filter != NULL)
		delete filter;
	filter = NULL;
	size_t numberTaps = (transformProperties.fftSize*transformProperties.overlap_percentage/100)+1;
	if(!(numberTaps%2))
		numberTaps = numberTaps-1;
	numFilterTaps = numberTaps;

	filter = new R2C(transformProperties.lowCutoff, transformProperties.highCutoff, transformProperties.transitionWidth, numberTaps, transformProperties.fftSize);
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
	retService = singleService(dataChar);
	retService = retService || singleService(dataOctet);
	retService = retService || singleService(dataShort);
	retService = retService || singleService(dataUshort);
	retService = retService || singleService(dataFloat);
	retService = retService || singleService(dataDouble);

	if (retService) {
		return NORMAL;
	}

	return NOOP;
}



int DataConverter_i::calculate_memorySize(int inputAmount){
	int value = 0;
	int size = 0;

	if (fftType == 2){
		value = ceil(inputAmount/((transformProperties.fftSize * transformProperties.overlap_percentage/100)));
		size = value*transformProperties.fftSize;
	}else if (fftType == 1){
		value = ceil(inputAmount/((transformProperties.fftSize * transformProperties.overlap_percentage/100)));
		size = value*transformProperties.fftSize;
	}else{
		size = inputAmount;
	}
	if(maxTransferSize == 0)
		_transferSize = size;
	return size;
}

void DataConverter_i::setupFFT(){
	//if passive
	if (outputType == 0){
		outputMode = inputMode;
		fftType = 0;
		//if input is Real and output is Complex
	}else if ( inputMode == 0 && outputType == 2){
		fftType = 2;
		outputMode = 1;
		//if input is Complex and outputType is Real
	}else if ( inputMode == 1 && outputType == 1){
		fftType = 1;
		outputMode = 0;
		//default to passive
	}else{
		fftType = 0;
		outputMode = inputMode;
	}

}

void DataConverter_i::configureSRI(BULKIO::StreamSRI *sri) {
	sampleRate = 1 / sri->xdelta;
	inputMode = sri->mode;
	setupFFT();

	double cf_offset = 0;
	if(fftType == 1){
		sampleRate = 2*sampleRate;
		cf_offset = sampleRate/4;
		sri->xdelta = 1.0/sampleRate;
	}else if(fftType == 2){
		if (transformProperties.tune_fs_over_4){
			cf_offset = -sampleRate/4;
			sampleRate = sampleRate/2;
		}
		else
			sampleRate = sampleRate;
		sri->xdelta = 1.0/sampleRate;
	}

	for (unsigned int i = 0; i < sri->keywords.length(); ++i){
		if(std::string(sri->keywords[i].id) == "COL_RF" && advancedSRI.update_col_rf_keyword){
			CORBA::Double COL_RF = 0;
			sri->keywords[i].value >>= COL_RF;
			COL_RF += cf_offset;
			sri->keywords[i].value <<= COL_RF;
		}
		else if(std::string(sri->keywords[i].id) == "CHAN_RF" && advancedSRI.update_chan_rf_keyword){
			CORBA::Double CHAN_RF = 0;
			sri->keywords[i].value >>= CHAN_RF;
			CHAN_RF += cf_offset;
			sri->keywords[i].value <<= CHAN_RF;
		}
	}
	sri->mode = outputMode;
	if (dataChar_out->isActive())
		dataChar_out->pushSRI(*sri);
	if (dataOctet_out->isActive())
		dataOctet_out->pushSRI(*sri);
	if (dataShort_out->isActive())
		dataShort_out->pushSRI(*sri);
	if (dataUshort_out->isActive())
		dataUshort_out->pushSRI(*sri);
	if (dataFloat_out->isActive())
		dataFloat_out->pushSRI(*sri);
	if (_outDoublePort->isActive()) 
		_outDoublePort->pushSRI(*sri);
}

int DataConverter_i::fft_execute(float* data, int size, bool EOS){
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
	//}

	//always 2
	//int elements_per_sample_output = 2;



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
		//int elements_to_process = std::min(size_avail,int(transformProperties.fftSize*elements_per_sample_input));
		//move data into processing buffer
		if (elements_to_process > 0){
			//std::cout << " elements to process is " << elements_to_process << " remainder_v is " << remainder_v << " readIndex is " << _readIndex << std::endl;

			// if elements_to_process is smaller than the fftSize-overlapsize, copy onto the buffer, and continue
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
				memcpy(&fftRBuffer[0],(float*)&fftBuffRealPtr[overlapRemovalPosition*elements_per_sample_input],sizeof(float)*overlap*elements_per_sample_input);
				fftwf_execute_dft_r2c(r2c,(float*)&fftBuffer[0],workingBuffer);
				filter->workFreq((fftwf_complex*)&workingBuffer[0],(fftwf_complex*)&workingBuffer[transformProperties.fftSize/2],(fftwf_complex*)&r2cTempBuffer[0]);
			} else {
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
				fftwf_execute_dft(c2c_r,r2cTempBuffer,complexTempBuffer);


				if(transformProperties.tune_fs_over_4){
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
					//fwrite((fftwf_complex*)&transformedBuffer[_writeIndex],sizeof(fftwf_complex),overlapRemovalPosition/2,debugR2C);
					//set size of _writeIndex
					_writeIndex += overlapRemovalPosition/2;
					dataAmount += overlapRemovalPosition/2 * elements_per_sample_output;
				}else{

					memcpy(&transformedBuffer[_writeIndex],&complexTempBuffer[overlap],sizeof(float)*overlapRemovalPosition*elements_per_sample_output);
					_writeIndex += overlapRemovalPosition;
					dataAmount += overlapRemovalPosition * elements_per_sample_output;
				}
			}else{
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
	return dataAmount;
}

void DataConverter_i::updateTimeStamp() {
	_FFTtimestampSec = _timestamp.twsec;
	_FFTtimestampFractionalSec = _timestamp.tfsec + (_samplesSinceLastTimestamp * ((double) 1 / (double) (sampleRate)));
	if (_FFTtimestampFractionalSec >= 1.0) {
		_FFTtimestampSec += floor(_FFTtimestampFractionalSec);
		_FFTtimestampFractionalSec =  (double) _FFTtimestampFractionalSec - floor(_FFTtimestampFractionalSec);
	}
		_timestamp.twsec = _FFTtimestampSec;
		_timestamp.tfsec = _FFTtimestampFractionalSec;
}

void DataConverter_i::moveTimeStamp(int shiftBack){
	//save off this timestamp in case there is a remainder from the previous
	//new timestamp, but data still left in buffer from previous one
	//modify the timestamp to account for the first sample from the old buffer
	if (shiftBack != 0){
		double temp = 0;
		temp = (double)shiftBack * (1.0/sampleRate);
		if (temp > _timestamp.tfsec){
			--_timestamp.twsec;
			_timestamp.tfsec = (1.0 + _timestamp.tfsec) - temp;
		}else{
			_timestamp.tfsec = _timestamp.tfsec - temp;
		}
	}
}

void DataConverter_i::adjustTimeStamp(BULKIO::PrecisionUTCTime& timestamp, BULKIO::PrecisionUTCTime& localtimestamp){

    size_t numberTaps = (transformProperties.fftSize*transformProperties.overlap_percentage/100)+1;
	if(!(numberTaps%2))
		numberTaps = numberTaps-1;
	numFilterTaps = numberTaps;
	double grpDelay = ((double)numFilterTaps)/(2.0*sampleRate);
	localtimestamp = timestamp;
	//adjust the group delay
	localtimestamp.tfsec += grpDelay;

	if (localtimestamp.tfsec >= 1.0){
		localtimestamp.tfsec -= 1.0;
		localtimestamp.twsec += 1.0;
	} else if (localtimestamp.tfsec <= 1.0) {
        localtimestamp.tfsec += 1.0;
		localtimestamp.twsec -= 1.0;        
    }

}
