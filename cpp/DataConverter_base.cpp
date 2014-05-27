/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of FilterDecimate.
 *
 * FilterDecimate is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * FilterDecimate is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
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

    dataChar = new bulkio::InCharPort("dataChar");
    addPort("dataChar", dataChar);
    dataOctet = new bulkio::InOctetPort("dataOctet");
    addPort("dataOctet", dataOctet);
    dataShort = new bulkio::InShortPort("dataShort");
    addPort("dataShort", dataShort);
    dataUshort = new bulkio::InUShortPort("dataUshort");
    addPort("dataUshort", dataUshort);
    dataFloat = new bulkio::InFloatPort("dataFloat");
    addPort("dataFloat", dataFloat);
    dataDouble = new bulkio::InDoublePort("dataDouble");
    addPort("dataDouble", dataDouble);
    dataChar_out = new bulkio::OutCharPort("dataChar_out");
    addPort("dataChar_out", dataChar_out);
    dataOctet_out = new bulkio::OutOctetPort("dataOctet_out");
    addPort("dataOctet_out", dataOctet_out);
    dataShort_out = new bulkio::OutShortPort("dataShort_out");
    addPort("dataShort_out", dataShort_out);
    dataUshort_out = new bulkio::OutUShortPort("dataUshort_out");
    addPort("dataUshort_out", dataUshort_out);
    dataFloat_out = new bulkio::OutFloatPort("dataFloat_out");
    addPort("dataFloat_out", dataFloat_out);
    dataDouble_out = new bulkio::OutDoublePort("dataDouble_out");
    addPort("dataDouble_out", dataDouble_out);
}

DataConverter_base::~DataConverter_base()
{
    delete dataChar;
    dataChar = 0;
    delete dataOctet;
    dataOctet = 0;
    delete dataShort;
    dataShort = 0;
    delete dataUshort;
    dataUshort = 0;
    delete dataFloat;
    dataFloat = 0;
    delete dataDouble;
    dataDouble = 0;
    delete dataChar_out;
    dataChar_out = 0;
    delete dataOctet_out;
    dataOctet_out = 0;
    delete dataShort_out;
    dataShort_out = 0;
    delete dataUshort_out;
    dataUshort_out = 0;
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
    addProperty(outputType,
                0,
                "outputType",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(maxTransferSize,
                0,
                "maxTransferSize",
                "",
                "readwrite",
                "samples",
                "external",
                "configure");

    addProperty(scaleOutput,
                scaleOutput_struct(),
                "scaleOutput",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(normalize_floating_point,
                normalize_floating_point_struct(),
                "normalize_floating_point",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(floatingPointRange,
                floatingPointRange_struct(),
                "floatingPointRange",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(advancedSRI,
                advancedSRI_struct(),
                "advancedSRI",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(transformProperties,
                transformProperties_struct(),
                "transformProperties",
                "",
                "readwrite",
                "",
                "external",
                "configure");

}


