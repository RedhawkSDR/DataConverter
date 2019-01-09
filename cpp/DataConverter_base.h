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

#ifndef DATACONVERTER_BASE_IMPL_BASE_H
#define DATACONVERTER_BASE_IMPL_BASE_H

#include <boost/thread.hpp>
#include <ossie/Component.h>
#include <ossie/ThreadedComponent.h>

#include <bulkio/bulkio.h>
#include "struct_props.h"

namespace enums {
    // Enumerated values for outputType
    namespace outputType {
        static const short Complex = 2;
        static const short Real = 1;
        static const short PassThrough = 0;
    }
}

class DataConverter_base : public Component, protected ThreadedComponent
{
    public:
        DataConverter_base(const char *uuid, const char *label);
        ~DataConverter_base();

        void start() throw (CF::Resource::StartError, CORBA::SystemException);

        void stop() throw (CF::Resource::StopError, CORBA::SystemException);

        void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

        void loadProperties();

    protected:
        // Member variables exposed as properties
        /// Property: outputType
        short outputType;
        /// Property: maxTransferSize
        CORBA::Long maxTransferSize;
        /// Property: scaleOutput
        scaleOutput_struct scaleOutput;
        /// Property: normalize_floating_point
        normalize_floating_point_struct normalize_floating_point;
        /// Property: floatingPointRange
        floatingPointRange_struct floatingPointRange;
        /// Property: advancedSRI
        advancedSRI_struct advancedSRI;
        /// Property: transformProperties
        transformProperties_struct transformProperties;

        // Ports
        /// Port: dataChar_in
        bulkio::InCharPort *dataChar_in;
        /// Port: dataOctet_in
        bulkio::InOctetPort *dataOctet_in;
        /// Port: dataShort_in
        bulkio::InShortPort *dataShort_in;
        /// Port: dataUshort_in
        bulkio::InUShortPort *dataUshort_in;
        /// Port: dataFloat_in
        bulkio::InFloatPort *dataFloat_in;
        /// Port: dataDouble_in
        bulkio::InDoublePort *dataDouble_in;
        /// Port: dataChar_out
        bulkio::OutCharPort *dataChar_out;
        /// Port: dataOctet_out
        bulkio::OutOctetPort *dataOctet_out;
        /// Port: dataShort_out
        bulkio::OutShortPort *dataShort_out;
        /// Port: dataUshort_out
        bulkio::OutUShortPort *dataUshort_out;
        /// Port: dataFloat_out
        bulkio::OutFloatPort *dataFloat_out;
        /// Port: dataDouble_out
        bulkio::OutDoublePort *dataDouble_out;

    private:
};
#endif // DATACONVERTER_BASE_IMPL_BASE_H
