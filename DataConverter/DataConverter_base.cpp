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
#include "DataConverter_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

DataConverter_base::DataConverter_base(const char *uuid, const char *label) :
    Resource_impl(uuid, label),
    ThreadedComponent()
{
    loadProperties();

    dataOctet = new bulkio::InOctetPort("dataOctet");
    addPort("dataOctet", dataOctet);
    dataUshort = new bulkio::InUShortPort("dataUshort");
    addPort("dataUshort", dataUshort);
    dataShort = new bulkio::InShortPort("dataShort");
    addPort("dataShort", dataShort);
    dataUlong = new bulkio::InULongPort("dataUlong");
    addPort("dataUlong", dataUlong);
    dataLong = new bulkio::InLongPort("dataLong");
    addPort("dataLong", dataLong);
    dataFloat = new bulkio::InFloatPort("dataFloat");
    addPort("dataFloat", dataFloat);
    dataDouble = new bulkio::InDoublePort("dataDouble");
    addPort("dataDouble", dataDouble);
    dataOctet_out = new bulkio::OutOctetPort("dataOctet_out");
    addPort("dataOctet_out", dataOctet_out);
    dataShort_out = new bulkio::OutShortPort("dataShort_out");
    addPort("dataShort_out", dataShort_out);
    dataUshort_out = new bulkio::OutUShortPort("dataUshort_out");
    addPort("dataUshort_out", dataUshort_out);
    dataLong_out = new bulkio::OutLongPort("dataLong_out");
    addPort("dataLong_out", dataLong_out);
    dataUlong_out = new bulkio::OutULongPort("dataUlong_out");
    addPort("dataUlong_out", dataUlong_out);
    dataFloat_out = new bulkio::OutFloatPort("dataFloat_out");
    addPort("dataFloat_out", dataFloat_out);
    dataDouble_out = new bulkio::OutDoublePort("dataDouble_out");
    addPort("dataDouble_out", dataDouble_out);
}

DataConverter_base::~DataConverter_base()
{
    delete dataOctet;
    dataOctet = 0;
    delete dataUshort;
    dataUshort = 0;
    delete dataShort;
    dataShort = 0;
    delete dataUlong;
    dataUlong = 0;
    delete dataLong;
    dataLong = 0;
    delete dataFloat;
    dataFloat = 0;
    delete dataDouble;
    dataDouble = 0;
    delete dataOctet_out;
    dataOctet_out = 0;
    delete dataShort_out;
    dataShort_out = 0;
    delete dataUshort_out;
    dataUshort_out = 0;
    delete dataLong_out;
    dataLong_out = 0;
    delete dataUlong_out;
    dataUlong_out = 0;
    delete dataFloat_out;
    dataFloat_out = 0;
    delete dataDouble_out;
    dataDouble_out = 0;
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void DataConverter_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    Resource_impl::start();
    ThreadedComponent::startThread();
}

void DataConverter_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    Resource_impl::stop();
    if (!ThreadedComponent::stopThread()) {
        throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
    }
}

void DataConverter_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the component running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    Resource_impl::releaseObject();
}

void DataConverter_base::loadProperties()
{
    addProperty(Octet,
                Octet_struct(),
                "Octet",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Ushort,
                Ushort_struct(),
                "Ushort",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Short,
                Short_struct(),
                "Short",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(ULong,
                ULong_struct(),
                "ULong",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Long,
                Long_struct(),
                "Long",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Float,
                Float_struct(),
                "Float",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Double,
                Double_struct(),
                "Double",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Octet_out,
                Octet_out_struct(),
                "Octet_out",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Ushort_out,
                Ushort_out_struct(),
                "Ushort_out",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Short_out,
                Short_out_struct(),
                "Short_out",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Ulong_out,
                Ulong_out_struct(),
                "Ulong_out",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Long_out,
                Long_out_struct(),
                "Long_out",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Float_out,
                Float_out_struct(),
                "Float_out",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Double_out,
                Double_out_struct(),
                "Double_out",
                "",
                "readwrite",
                "",
                "external",
                "configure");

}


