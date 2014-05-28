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

#ifndef DATACONVERTER_IMPL_BASE_H
#define DATACONVERTER_IMPL_BASE_H

#include <boost/thread.hpp>
#include <ossie/Resource_impl.h>
#include <ossie/ThreadedComponent.h>

#include <bulkio/bulkio.h>
#include "struct_props.h"

class DataConverter_base : public Resource_impl, protected ThreadedComponent
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
        short outputType;
        CORBA::Long maxTransferSize;
        scaleOutput_struct scaleOutput;
        normalize_floating_point_struct normalize_floating_point;
        floatingPointRange_struct floatingPointRange;
        advancedSRI_struct advancedSRI;
        transformProperties_struct transformProperties;

        // Ports
        bulkio::InCharPort *dataChar;
        bulkio::InOctetPort *dataOctet;
        bulkio::InShortPort *dataShort;
        bulkio::InUShortPort *dataUshort;
        bulkio::InFloatPort *dataFloat;
        bulkio::InDoublePort *dataDouble;
        bulkio::OutCharPort *dataChar_out;
        bulkio::OutOctetPort *dataOctet_out;
        bulkio::OutShortPort *dataShort_out;
        bulkio::OutUShortPort *dataUshort_out;
        bulkio::OutFloatPort *dataFloat_out;
        bulkio::OutDoublePort *dataDouble_out;

    private:
};
#endif // DATACONVERTER_IMPL_BASE_H
