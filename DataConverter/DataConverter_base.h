/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This file is part of REDHAWK Basic Components DataConverter.
 *
 * REDHAWK Basic Components DataConverter is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * REDHAWK Basic Components DataConverter is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this
 * program.  If not, see http://www.gnu.org/licenses/.
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
        Octet_struct Octet;
        Ushort_struct Ushort;
        Short_struct Short;
        ULong_struct ULong;
        Long_struct Long;
        Float_struct Float;
        Double_struct Double;
        Octet_out_struct Octet_out;
        Ushort_out_struct Ushort_out;
        Short_out_struct Short_out;
        Ulong_out_struct Ulong_out;
        Long_out_struct Long_out;
        Float_out_struct Float_out;
        Double_out_struct Double_out;

        // Ports
        bulkio::InOctetPort *dataOctet;
        bulkio::InUShortPort *dataUshort;
        bulkio::InShortPort *dataShort;
        bulkio::InULongPort *dataUlong;
        bulkio::InLongPort *dataLong;
        bulkio::InFloatPort *dataFloat;
        bulkio::InDoublePort *dataDouble;
        bulkio::OutOctetPort *dataOctet_out;
        bulkio::OutShortPort *dataShort_out;
        bulkio::OutUShortPort *dataUshort_out;
        bulkio::OutLongPort *dataLong_out;
        bulkio::OutULongPort *dataUlong_out;
        bulkio::OutFloatPort *dataFloat_out;
        bulkio::OutDoublePort *dataDouble_out;

    private:
};
#endif // DATACONVERTER_IMPL_BASE_H
